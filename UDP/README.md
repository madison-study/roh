## HTTP vs Socket

**HTTP(HyperText Transfer Protocol)** is a protocol used to transfer HTML, JSON, and other web content.
It operates on a request-response model:

* The client sends a request,

* The server replies with a response.

> 📌 This means the server can only respond when the client initiates a request,
> so it is a one-way (half-duplex) communication.

**Socket** communication supports two-way (full-duplex) communication.
Once connected, the client and server can *freely exchange data in both directions*.

> composed of protocol, ip, port number

Commonly used in real-time chat, online games, push notifications, etc.

TCP sockets maintain persistent connections and provide reliable data transfer.

| Feature           | HTTP                       | Socket                           |
| ----------------- | -------------------------- | -------------------------------- |
| Communication     | Request-Response (one-way) | Two-way (full-duplex)            |
| Connection        | Established per request    | Persistent connection            |
| Real-time Support | Low                        | High                             |
| Typical Use Cases | Web pages, REST APIs       | Chat apps, games, real-time apps |
| Protocol          | HTTP                       | TCP / UDP                        |


## Network Socket

> Socket = Protocol + IP address + Port number

In Unix-like systems, **a socket is treated as a type of file**, though with some differences in Windows.

Everything is a File (in Unix)
Unix systems follow the philosophy that ***"everything is a file"***, including not only regular files and directories, but also devices, pipes, and sockets.

### Socket as a File Descriptor

When a process creates a socket using

```c
// AF_INET: Adress Familiy meaning IPv4 internet protocol AF_INET6 -> IPv6
// SOCK_STREAM: Socket Type SOCK_STREAM: TCP, SOCK_DGRAM: UDP
// 0: specify protocol 0 means choose default protocol based on prev args in this case TCP
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
```

the `sockfd` returned is a file descriptor, just like the one returned by open() for a regular file. 
This allows sockets to be handled using standard I/O system calls

### Prepare Server address struct (Client)

process has to notify OS server's IP address & port

```c
// af: AF_INET IPv4 AF_INET IPv6
// src: string based IP addr
// dst: destination for binary translated IP address
//
// return 1 on success 0 on invalid IP addr (format err) -1 on af not supported
int inet_pton(int af, const char *src, void *dst);
```

### Send MSG using socket (Client UDP) 

```c
char *message = "Hello, UDP Server!";
ssize_t sent_bytes = sendto(sockfd, message, strlen(message), 0,
                            (struct sockaddr*)&servaddr, sizeof(servaddr));

if (sent_bytes < 0) {
    perror("sendto failed");
}
```

when `sendto()` is called process send data to OS, OS send UDP header + IP header to network interface 

* function returns after request (not synchronous)

### Receive MSG using socket (Server UDP)

```c
struct sockaddr_in cliaddr;
socklen_t len = sizeof(cliaddr);
char buffer[1024];

ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                     (struct sockaddr*)&cliaddr, &len);
if (n < 0) {
    perror("recvfrom failed");
} else {
    buffer[n] = '\0';
    printf("Received message: %s\n", buffer);
}
```

`recvfrom()` function copy UDP packet to buffer and return sender's address information

## UDP

**UDP(User Datagram Protocol)** is connectionless protocol, which is designed for fast data transfer

### 🔄 Work Flow

* **sender**

1. create socket `socket(AF_INET, SOCK_DGRAM, 0)`

2. set server address struct

3. send message

* **receiver**

파일 디스크립터 (file descriptor)
프로세스가 소켓을 생성하면, 일반 파일처럼 파일 디스크립터(fd) 를 통해 소켓을 참조합니다. 예를 들어:

int sockfd = socket(AF_INET, SOCK_STREAM, 0);
위에서 sockfd는 파일 디스크립터이며, 일반 파일에서 open()이 반환하는 것과 동일한 방식으로 사용됩니다.

read/write 사용 가능
소켓은 read(), write() 또는 send(), recv() 등을 사용해 입출력이 가능합니다.

/proc/[pid]/fd/ 경로에서 확인 가능
리눅스에서는 특정 프로세스가 열고 있는 파일 디스크립터를 /proc/[pid]/fd/ 경로에서 확인할 수 있습니다. 여기에 소켓도 링크로 나타납니다:

ls -l /proc/1234/fd/
lrwx------ 1 user user 64 May 20 00:00 3 -> 'socket:[12345]'
bind()에서 파일 경로 사용 (Unix domain socket)
TCP/IP 소켓은 IP와 포트를 사용하지만, 유닉스 도메인 소켓은 실제 파일 시스템 경로를 사용합니다:


struct sockaddr_un addr;
addr.sun_family = AF_UNIX;
strcpy(addr.sun_path, "/tmp/mysocket");

bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
이 경우 /tmp/mysocket는 실제로 파일 시스템에 나타나는 소켓 파일입니다.

## UDP

UDP protocol