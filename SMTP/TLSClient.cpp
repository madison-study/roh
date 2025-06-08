#include "TLSClient.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

TLSClient::TLSClient(const std::string &domain, uint16_t port)
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx)
        throw std::runtime_error("Failed to create SSL_CTX");

    std::string ip = get_best_mx_record(domain);
    printf("=====================\n");
    std::cout << ip << std::endl;
    printf("=====================\n");
    createSocketAndConnect(ip, port);
    printf("=====================\n");
    ssl = SSL_new(ctx);
    if (!ssl)
    {
        SSL_CTX_free(ctx);
        close(sock);
        throw std::runtime_error("Failed to create SSL structure");
    }

    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0)
    {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        close(sock);
        throw std::runtime_error("Failed SSL handshake");
    }
}

TLSClient::~TLSClient()
{
    if (ssl)
    {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    if (ctx)
        SSL_CTX_free(ctx);
    if (sock >= 0)
        close(sock);
}

std::string TLSClient::get_best_mx_record(const std::string &domain)
{
    int len = res_query(domain.c_str(), ns_c_in, ns_t_mx, response, sizeof(response));
    if (len < 0)
    {
        perror("res_query failed");
        return "";
    }

    ns_msg handle;
    if (ns_initparse(response, len, &handle) < 0)
    {
        perror("ns_initparse failed");
        return "";
    }

    int count = ns_msg_count(handle, ns_s_an);

    int best_pref = std::numeric_limits<int>::max();
    std::string best_mx;

    for (int i = 0; i < count; ++i)
    {
        ns_rr rr;
        if (ns_parserr(&handle, ns_s_an, i, &rr) < 0)
        {
            perror("ns_parserr failed");
            continue;
        }

        const u_char *rdata = ns_rr_rdata(rr);
        if (ns_rr_rdlen(rr) < 3)
            continue; // MX 레코드는 최소 3바이트 이상이어야 함

        int preference = (rdata[0] << 8) | rdata[1]; // MX preference 값

        char exchange[NS_MAXDNAME];
        if (dn_expand(response, response + len, rdata + 2, exchange, sizeof(exchange)) < 0)
        {
            perror("dn_expand failed");
            continue;
        }

        if (preference < best_pref)
        {
            best_pref = preference;
            best_mx = exchange;
        }
    }

    std::cout << best_mx << std::endl;
    best_mx = "smtp.gmail.com";
    len = res_query(best_mx.c_str(), ns_c_in, ns_t_a, response, sizeof(response));
    if (len < 0)
    {
        perror("res_query failed");
        return "";
    }

    if (ns_initparse(response, len, &handle) < 0)
    {
        perror("ns_initparse failed");
        return "";
    }
    count = ns_msg_count(handle, ns_s_an);
    for (int i = 0; i < count; ++i)
    {
        ns_rr rr;
        if (ns_parserr(&handle, ns_s_an, i, &rr) < 0)
        {
            perror("ns_parserr failed");
            continue;
        }

        const u_char *rdata = ns_rr_rdata(rr);
        char ip[INET6_ADDRSTRLEN];

        if (ns_rr_type(rr) == ns_t_a && ns_rr_rdlen(rr) == 4)
        {
            // A record (IPv4)
            inet_ntop(AF_INET, rdata, ip, sizeof(ip));
            return ip;
        }
        else if (ns_rr_type(rr) == ns_t_aaaa && ns_rr_rdlen(rr) == 16)
        {
            // AAAA record (IPv6)
            inet_ntop(AF_INET6, rdata, ip, sizeof(ip));
            return ip;
        }
    }

    return "";
}

void TLSClient::createSocketAndConnect(const std::string &ip, uint16_t port)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        throw std::runtime_error("Failed to create socket");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
    {
        close(sock);
        throw std::invalid_argument("Invalid IP address");
    }

    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("connect");
        close(sock);
        throw std::runtime_error("Failed to connect");
    }
}

void TLSClient::sendMessage(const std::string &message)
{
    size_t totalSent = 0;
    size_t len = message.size();

    while (totalSent < len)
    {
        int sent = SSL_write(ssl, message.data() + totalSent, (int)(len - totalSent));
        if (sent <= 0)
        {
            throw std::runtime_error("SSL_write failed");
        }
        totalSent += sent;
    }
}

std::string TLSClient::recvMessage(size_t bufSize)
{
    char buffer[bufSize + 1];
    int received = SSL_read(ssl, buffer, (int)bufSize);
    if (received <= 0)
    {
        throw std::runtime_error("SSL_read failed or connection closed");
    }
    buffer[received] = '\0';
    return std::string(buffer);
}