#ifndef __CLIENT_
#define __CLIENT_
#include<cstdio>
#include<iostream>
#include<sstream>
#include<cstdlib>
#include<string>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<fcntl.h>
#include<pthread.h>
#include<errno.h>
#include"../../include/page_rank.h"
#include"log.h"
#define RECV_SIZE 5000000
#define IP "192.168.1.191" 
#define PORT "1030"
#define COLLAPSE 4
#define ERRMSG "你所查询的页面不存在或者已经被删除!"
#define RIGHTMSG "输入正确"
namespace MY_CLIENT
{
    class My_client
    {
        public:
            static My_client* getInstance(const std::string& ip,const std::string& port);
            My_client(const std::string& ip,const std::string& port);
            void socket_init();
            int set_nonblock(int& sockfd);
            void socket_error(const std::string& message,const int& value);
            int socket_connect();
            int recv_msg(std::string& result);
            int recvn(const int& server_sock,char* p,const int& send_len);
            int send_msg(const char* msg);
            int sendn(const int& server_sock,const char* p,const int& send_len);
            void trim_space(std::string& line);
            int err_search(const std::string& result);
            int not_null(const std::string& word);
            int handle_word(std::string& search,std::string& result);
            int handle_title(std::string& result);
            void handle_content(std::string& result);
            void handle_page();
        private:
            int server_sock;
            struct sockaddr_in server_addr;
            char message[RECV_SIZE];
            const std::string m_ip;
            const int m_port;
            Json::Reader reader;
            Json::FastWriter writer;
            std::string search;
            std::string result;
            static My_client* m_ptr;
            static MUTEX::CMutex m_lock;
    };
}
#endif
