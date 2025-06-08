#include <resolv.h>
#include <arpa/nameser.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <limits>
#include <arpa/inet.h>

std::string get_best_mx_record(const std::string &domain)
{
    u_char response[NS_PACKETSZ];

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
    std::cout<< count << std::endl;
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

int main()
{
    std::string domain = "naver.com";
    std::string best_mx = get_best_mx_record(domain);
    if (!best_mx.empty())
        std::cout << "Best MX for " << domain << ": " << best_mx << std::endl;
    else
        std::cout << "Failed to get MX record for " << domain << std::endl;

    return 0;
}
