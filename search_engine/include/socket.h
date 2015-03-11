#ifndef __MY_SOCKET_H
#define __MY_SOCKET_H
#include<string>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#define MAXSIZE 1024
namespace MY_SOCKET
{
    struct  My_socket
    {
        struct sockaddr_in m_client_addr;
        char m_search[MAXSIZE];
        socklen_t m_client_addr_len;
        My_socket(struct sockaddr_in& client_addr,socklen_t& client_addr_len,char* msg);
    };
}
#endif
