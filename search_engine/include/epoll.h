#ifndef __EPOLL_H
#define __EPOLL_H
#include<iostream>
#include"thread_pool.h"
#include"conf.h"
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include"query_handle.h"
#define EPOLLSIZE 1024
namespace MY_EPOLL
{
    class My_epoll
    {
        public:
        /*    My_epoll(const std::string& conf_path):m_conf(conf_path),m_ip(m_conf.config["my_ip"]),m_port(atoi(m_conf.config["my_port"].c_str())),m_log(m_conf.config["my_log"])
        {
            m_log.init(); 
        }*/
            My_epoll(const std::string& conf_path,const std::string& port):m_conf(conf_path),m_ip(m_conf.config["my_ip"]),m_port(atoi(port.c_str())),m_log(m_conf.config["my_log"])
        {
            m_log.init(); 
        }
            void socket_init();
            int recv_msg(char* msg,const int& fd_client);
            void handle_query();
            int  set_nonblock(int& sockfd);
            void epoll_add(int epollfd,int sockfd,int state);
            void socket_error(const std::string& message,const int& value);
            int socket_accept();
        private:
            MY_CONF::CMyconf m_conf;
            int server_sock;
            struct sockaddr_in server_addr;
            const std::string m_ip;
            const int m_port;
            LOG_SYSTEM::Log_system m_log;
            My_epoll& operator=(My_epoll& obj);
            My_epoll(My_epoll& obj);
    };
}
#endif
