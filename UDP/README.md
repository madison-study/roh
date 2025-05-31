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

## 🌐 UDP (User Datagram Protocol)

**UDP** is a **connectionless** transport-layer protocol designed for fast and lightweight data transmission.  
Unlike TCP, it does not guarantee delivery, order, or error checking — making it ideal for real-time applications like video streaming or online gaming.

---

### 🔄 Work Flow

#### 📨 Sender (Client Side)

1. **Create Socket**
   ```c
   int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   ```

2. set server address struct
    ```c
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    ```

3. send message
    ```c
    void UdpClient::sendMessage(const std::string &message)
    {
        ssize_t sent = sendto(sock, message.c_str(), message.length(), 0,
                            (sockaddr *)&serverAddr, sizeof(serverAddr));
        if (sent < 0)
        {
            perror("Send failed");
            throw std::runtime_error("Failed to send message");
        }
    }
    ```

#### 📥 Receiver (Server Side)

1. create socket
    ```c
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    ```

2. set server address struct

    ```c
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 모든 IP로부터 수신 허용
    serverAddr.sin_port = htons(port);
    ```

3. bind

    Bind the socket to a local IP and port:
    
    ```c
    bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr));
    ```

4. Receive MSG
    ```c
    void UdpServer::receiveMessage()
    {
        char buffer[BUFFER_SIZE];
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        std::memset(buffer, 0, BUFFER_SIZE);
        ssize_t received = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                    (sockaddr *)&clientAddr, &clientLen);
        if (received < 0)
        {
            perror("recvfrom failed");
            return;
        }

        buffer[received] = '\0'; // null-terminate the string
        std::cout << "Received from "
                << inet_ntoa(clientAddr.sin_addr)
                << ":" << ntohs(clientAddr.sin_port)
                << " -> " << buffer << std::endl;
    }
    ```


### Error Detection in UDP

1. UDP Checksum
    UDP provides a checksum field to detect data corruption.

    Location: 4th field in the UDP header (16 bits total)

    Coverage: UDP header + data + parts of the IP header (pseudo header)

    Functionality:
    The sender calculates the checksum before transmission and inserts it into the header.

    The receiver recalculates the checksum on arrival; if the value differs, the packet is considered corrupted.

    Optional in IPv4, but mandatory in IPv6.

2. Detection Only, No Recovery
    UDP can detect errors, but it cannot recover from them.

    Corrupted packets are discarded immediately.

    No retransmission or correction is provided by UDP.

    Applications must implement error-handling, retransmission, and acknowledgment logic if needed.

3. Pseudo Header
    To increase reliability, UDP includes a pseudo header in the checksum calculation, which contains selected IP layer fields.

    Pseudo Header includes:
    Source IP address

    Destination IP address

    Protocol number (UDP = 17)

    UDP length

    This helps detect errors like incorrect destination address during transmission.

#### 📌 Summary Table

| Item                      | UDP                      |
| ------------------------- | ------------------------ |
| Checksum                  | Yes (16-bit)             |
| Error Detection           | Yes                      |
| Error Recovery            | No                       |
| Guaranteed Delivery       | No                       |
| Retransmission            | No                       |
| Handling Missing Packets  | Delegated to Application |
| Checksum Required in IPv6 | Yes                      |


