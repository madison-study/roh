# 🌐 TCP (Transmission Control Protocol)

TCP is a **connection-oriented** transport-layer protocol that ensures reliable, ordered, and error-checked delivery of data between applications.

---

## Key Concepts

* Connection-oriented: set connection before transfer (3-way handshake)
* Reliability: resend lost packet, remove duplicated packet
* Flow Control: eg: sliding window
* Congestion Control: control transfer speed (eg: AIMD, Slow Start)

## 🤝 3-Way Handshake (Establish Connection)

1. SYN: Client -> Server request connection (including seq number)
2. SYN-ACK: Server -> Client respond (My seq + ACK for client seq)
3. ACK: Client -> Server 

after this data transfer begins

## 📤 Data Transfer

1. TCP divide data into **Segments** and attach seq number to each segment
2. Receiver responde with **ACK** notifying which data has arrived
3. If no **ACK** resend
4. order segments using seq number

## ❌ 4-Way HandShake (Close Connection)

1. FIN: One side sends a connection termination request (Finish).
2. ACK: The other side acknowledges the FIN.
3. FIN: The other side also sends its own termination request.
4. ACK: The original sender acknowledges the second FIN.


| 특징    | TCP                     | UDP                 |
| ----- | ----------------------- | ------------------- |
| Connection | Connection-based | Connectionless |
| Reliabilty   | O                      | X             |
| Speed    | slow                  | fast                  |
| usage | file transfer, web browsing  | streaming, game, voip     |

## 🏄 TCP is Stream

Client send: "12457810111314" but 

Server can show 
```bash
Received from 127.0.0.1:56600 -> 12
Received from 127.0.0.1:56600 -> 457810111314
```

> TCP guarantees order of stream, but does not guarantee size of chunk

user's responsibility to set boundary (eg: multipart form-data, size prefix)

### Prefix

attach size header to message  [ size ] [ message ]

## 🕒 Timeout

### Connect

if server does not answers `connect()` TCP waits and retries -> depends on OS setting

Example on Linux to control this:

```bash
/proc/sys/net/ipv4/tcp_syn_retries   # Default: 6 retries
```

### Data Transmission

* send() usually returns immediately once the data is copied to the OS buffer.

* If the send buffer is full, send() can block or fail depending on socket settings.

* recv() blocks if no data is available unless you set a timeout (SO_RCVTIMEO).

Automatic retransmission
* TCP automatically retransmits lost packets inside the kernel if ACKs are not received.

* This retransmission is invisible to your application — it just experiences delays or connection drop.

Linux controls retransmissions with:

```bash
/proc/sys/net/ipv4/tcp_retries2   # Default: 15 retries (can mean several minutes)
```

After this retry limit is reached without success, TCP closes the connection (RST).

### Keepalive

TCP keepalive probes are sent when the connection is idle, to check if the peer is alive.

If keepalive probes fail after a number of retries, TCP closes the connection.

## C++ code

### Connection
```cpp
    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Connect failed");
        close(sock);
        throw std::runtime_error("Failed to connect to server");
    }
```