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
| Speed    | fast                  | slow                  |
| usage | file transfer, web browsing  | streaming, game, voip     |

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