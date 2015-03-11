#include"epoll.h"
namespace MY_EPOLL
{
    int  My_epoll::set_nonblock(int& sockfd)
    {
        int old=fcntl(sockfd,F_GETFL);
        old|=O_NONBLOCK;
        fcntl(sockfd,F_SETFL,old);
        return 0;
    }
    void My_epoll::socket_init()
    {
        server_sock=socket(AF_INET,SOCK_STREAM,0);
        if(server_sock==-1)
        {
            m_log.error("server sock create failed");
        }
        memset(&server_addr,0,sizeof(server_addr));
        server_addr.sin_family=AF_INET;
        server_addr.sin_port=htons(m_port);
        server_addr.sin_addr.s_addr=inet_addr(m_ip.c_str());
        if(bind(server_sock,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1)
        {
            m_log.error("server socket binding failed");
            perror("server socket binding failed");
        }
        if(listen(server_sock,128)==-1)
        {
            m_log.error("server socket listen failed");
        }
    }
    int My_epoll::socket_accept()
    {
        struct sockaddr_in client_addr;
        memset(&client_addr,0,sizeof(client_addr));
        socklen_t length=sizeof(client_addr);
        int fd_client=accept(server_sock,(struct sockaddr*)&client_addr,&length);
        std::string error_msg="server accept failed";
        socket_error(error_msg,fd_client);
        std::stringstream s1;
        s1<<"main thread ID: "<<pthread_self()<<"connect client(IP"<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<")";
        m_log.notice(s1.str());
        return fd_client;
    }
    void My_epoll::socket_error(const std::string& err_msg,const int& value)
    {
        if(value==-1)
        {
            m_log.error(err_msg);
            perror(err_msg.c_str());
        }
    }
    void My_epoll::handle_query()
    {
        struct epoll_event events[EPOLLSIZE];
        int epollfd=epoll_create(EPOLLSIZE);
        if(epollfd==-1)
        {
            m_log.error("epoll create failed");
        }
        socket_init();
        set_nonblock(server_sock);
        QUERY_HANDLE::Query_handle handle(m_conf,&m_log);
        epoll_add(epollfd,server_sock,EPOLLIN|EPOLLET);
        std::cout<<"server wait for connecting......."<<std::endl;
        m_log.notice("server wait for connecting.........");
        while(1)
        {
            int ret=epoll_wait(epollfd,events,EPOLLSIZE,-1);
            for(int index=0;index<ret;index++)
            {
                if(events[index].data.fd==server_sock)
                {
                    int fd_client=socket_accept();                   
                    set_nonblock(fd_client);
                    handle(fd_client);
                }
            }
        }
    }
    void My_epoll::epoll_add(int epollfd,int sockfd,int state)
    {
        struct epoll_event event;
        event.events=state;
        event.data.fd=sockfd;
        epoll_ctl(epollfd,EPOLL_CTL_ADD,sockfd,&event);
    }
}
