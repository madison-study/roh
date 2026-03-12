#ifndef NAMESERVER_H
#define NAMESERVER_H

#define MAX_NAMESERVERS 10
#define MAX_NS_NAME 256
#define MAX_NS_IP 16

typedef struct {

    char name[MAX_NS_NAME];
    char ip[MAX_NS_IP];

} nameserver_t;

typedef struct {

    nameserver_t servers[MAX_NAMESERVERS];
    int count;

} nameserver_list_t;

#endif