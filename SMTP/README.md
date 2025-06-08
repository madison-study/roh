# SMTP( Simple Mail Transfer Protocol )

SMTP (Simple Mail Transfer Protocol) is the standard protocol for sending emails between mail servers.

* on OSI layer level 7

* SMTP is only for sending
* POP3/IMAP is used for reading emails

* port 25(MTA -> MTA), 587 (submission)
* Text based protocol

## SMTP vs POP3, IMAP

On email system there are two objects, *UA(User Agent)* and *MTA(Mail Transfer Agent)*. The UA refers to applications used for sending and receiving emails, such as Outlook, Thunderbird, or the default mail apps provided by OS. The MTA refers to email server. POP3, IMAP are protocols used by UA to retrieve and manage emails from the MTA. SMTP is used when UA requests the MTA to send an email and also manages communication between MTAs

## General Email Transfer Process

```bash
User → SMTP Server → Recipient's SMTP Server → Mailbox → POP3/IMAP → User reads email
```

## 📜 SMTP workflow & cmd

```txt
1. Connect to SMTP server (TCP)
2. HELO / EHLO
3. MAIL FROM:<sender@example.com>
4. RCPT TO:<recipient@example.com>
5. DATA → Email content
6. QUIT
```

| Command     | Meaning                      |
| ----------- | ---------------------------- |
| HELO / EHLO | Starts connection            |
| MAIL FROM:  | Specify sender email         |
| RCPT TO:    | Specify recipient email      |
| DATA        | Start entering email content |
| QUIT        | Close the connection         |
| RSET        | Reset the connection (when client sent wrong message) |

## MX Record

MX (Mail Exchange) records are a type of DNS (Domain Name System) record that tell the internet where to deliver emails for a specific domain.

### Workflow

When someone sends an email to user@example.com, the sender’s mail server needs to figure out which server handles mail for example.com.

* Records route email traffic to correct mail servers
* allow domains to have redundant mail servers for reliability 
* separate mail services from web services

It does this by:

1. Looking up the MX record for example.com using DNS.

2. The MX record responds with the hostname(s) of the mail server(s) that should receive the email.

```bash
nslookcup
> set q=mx
> domain.com

# The number is called the priority or preference value. Lower numbers have higher priority.
naver.com       mail exchanger = 20 mx6.mail.naver.com.
naver.com       mail exchanger = 10 mx2.naver.com.  //  lower number higher priority
naver.com       mail exchanger = 20 mx4.mail.naver.com.
naver.com       mail exchanger = 10 mx1.naver.com.
naver.com       mail exchanger = 10 mx3.naver.com.
naver.com       mail exchanger = 20 mx5.mail.naver.com.

```

## Domain -> IP

lookup MX record
`gmail.com → MX → gmail-smtp-in.l.google.com`

host name from MX record -> A/AAAA record

`gmail-smtp-in.l.google.com → A/AAAA → IP addr`

```bash
dig MX gmail.com

> gmail.com.  MX  5  gmail-smtp-in.l.google.com.

dig A gmail-smtp-in.l.google.com

> 74.125.203.27
```

## Can UA directly sends mail to receiver MTA

Technically possible but limited ( Spam filtering, Authentication )

* DKIM(DomainKeys Identified Mail)
    prevents spoofing (fake sender address)
* SPF record (Sender Policy Framework)  
    checks sender's ip address is allowed IP 


## SMTP over TLS (STARTTLS)

TLS: after SSL 3.0 

encrypt communication between server and client using TLS

----

* https://datatracker.ietf.org/doc/html/rfc5321
* https://hudi.blog/smtp-with-python/
