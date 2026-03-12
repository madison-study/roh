# DNS Resolver From Scratch

## 1. Overview

이 프로젝트는 **DNS(Domain Name System)**의 동작 원리를 이해하기 위해
**DNS Resolver를 직접 구현하는 것을 목표로 합니다.**

일반적으로 애플리케이션은 `getaddrinfo()` 같은 OS API를 통해
도메인 이름을 IP 주소로 변환합니다.

하지만 내부적으로는 다음과 같은 **DNS 조회 과정(DNS resolution)**이 수행됩니다.

```
Client
  ↓
Local DNS Resolver
  ↓
Root DNS Server
  ↓
TLD DNS Server (.com, .net ...)
  ↓
Authoritative DNS Server
  ↓
IP Address
```

이 프로젝트에서는 이 과정을 **직접 구현하여 DNS 조회가 어떻게 이루어지는지 이해하는 것**을 목표로 합니다.

---

# 2. What is DNS?

DNS(Domain Name System)는 **도메인 이름을 IP 주소로 변환하는 분산 데이터베이스 시스템**입니다.

예를 들어 사용자가 다음과 같은 URL에 접속한다고 가정합니다.

```
https://www.example.com
```

하지만 실제 인터넷 통신은 **IP 주소 기반**으로 이루어집니다.

```
www.example.com → 93.184.216.34
```

따라서 DNS는 **사람이 읽기 쉬운 도메인 이름을 네트워크가 사용할 수 있는 IP 주소로 변환하는 역할**을 합니다.

---

# 3. DNS Architecture

DNS는 **계층적(hierarchical) 구조**로 이루어져 있습니다.

```
                Root DNS
                   │
          ┌────────┴────────┐
          │                 │
        .com              .org
          │
      example.com
          │
     www.example.com
```

각 단계의 역할은 다음과 같습니다.

### 1. Root DNS Server

DNS 시스템의 최상위 서버입니다.

Root 서버는 특정 도메인의 IP를 직접 알고 있지 않고
대신 **TLD 서버의 위치를 알려줍니다.**

예:

```
Query: www.example.com
Response: ask .com TLD server
```

---

### 2. TLD (Top Level Domain) Server

TLD 서버는 `.com`, `.org`, `.net` 같은 **최상위 도메인**을 관리합니다.

예:

```
Query: www.example.com
Response: example.com authoritative server
```

---

### 3. Authoritative DNS Server

도메인의 **실제 DNS 레코드를 가지고 있는 서버**입니다.

예:

```
www.example.com → 93.184.216.34
```

이 서버가 최종적으로 **IP 주소를 반환합니다.**

---

# 4. DNS Resolution Process

DNS 조회 과정은 다음과 같이 이루어집니다.

### Step 1: Client Query

사용자가 브라우저에 도메인을 입력합니다.

```
www.example.com
```

애플리케이션은 DNS resolver에 요청을 보냅니다.

---

### Step 2: Root DNS Query

Resolver는 먼저 Root DNS 서버에 질의합니다.

```
Where is www.example.com ?
```

Root 서버는 다음과 같이 응답합니다.

```
Ask the .com TLD server
```

---

### Step 3: TLD Query

Resolver는 `.com` TLD 서버에 질의합니다.

```
Where is www.example.com ?
```

TLD 서버는 다음과 같이 응답합니다.

```
Ask the authoritative server for example.com
```

---

### Step 4: Authoritative Query

Resolver는 example.com의 authoritative server에 질의합니다.

```
Where is www.example.com ?
```

응답:

```
93.184.216.34
```

---

### Step 5: Response to Client

Resolver는 얻은 IP 주소를 클라이언트에 반환합니다.

```
www.example.com → 93.184.216.34
```

---

# 5. Recursive vs Iterative Queries

DNS 조회에는 두 가지 방식이 존재합니다.

### Recursive Query

Resolver가 **모든 DNS 조회를 대신 수행**합니다.

```
Client → Resolver → Root → TLD → Authoritative
```

Client는 최종 결과만 받습니다.

---

### Iterative Query

Resolver가 각 서버로부터 **다음에 어디로 가야 하는지 정보만 받습니다.**

```
Resolver → Root → TLD → Authoritative
```

이 프로젝트에서는 **Iterative Resolver**를 직접 구현합니다.

---

# 6. Project Goal

이 프로젝트의 목표는 다음과 같습니다.

* DNS packet 구조 이해
* UDP 기반 네트워크 통신 구현
* Iterative DNS resolution 구현
* Root → TLD → Authoritative lookup 구현
* DNS response parsing

---

# 7. Implementation Plan

프로젝트는 다음 단계로 구현합니다.

1. UDP socket을 이용한 DNS query 전송
2. DNS packet 생성
3. DNS response parsing
4. Root server query
5. TLD server query
6. Authoritative server query
7. Iterative DNS resolver 완성

---

# 8. Tech Stack

* Language: C
* Protocol: UDP
* Port: 53
* Standard: RFC 1035 (DNS)

---

# 9. Example

실행 예시:

```
$ ./resolver www.example.com
Querying root server...
Querying .com TLD server...
Querying authoritative server...

Result:
www.example.com → 93.184.216.34
```
