#include"client.h"
MY_CLIENT::My_client* MY_CLIENT::My_client::m_ptr=NULL;
MUTEX::CMutex MY_CLIENT::My_client::m_lock;
int null(const std::string& word)
{
    std::string line(word);
    int start=0;
    std::string exclude="\a\n\t\r\v\b\f ";
    while((start=(line.find_first_of(exclude,start)))!=std::string::npos)
    {
        line.erase(start, 1);
        start++;
    }
    if(line.size())
    {
        return 1;
    }
    return 0;
}
namespace MY_CLIENT
{
    My_client* My_client::getInstance(const std::string& ip,const std::string& port)
    {
        if(m_ptr==NULL)
        {
            m_lock.lock();
            if(m_ptr==NULL)
            {
                m_ptr=new My_client(ip,port);
            }
            m_lock.unlock();
        }
        return m_ptr;
    }
    My_client::My_client(const std::string& ip,const std::string& port):m_ip(ip),m_port(atoi(port.c_str()))
    {
        socket_init();
        socket_connect();
    }
    int My_client::err_search(const std::string& result)
    {
        if(result.find(RIGHTMSG)!=std::string::npos)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    int My_client::not_null(const std::string& word)
    {
        std::string line(word);
        int start=0;
        std::string exclude="\a\n\t\r\v\b\f ";
        while((start=(line.find_first_of(exclude,start)))!=std::string::npos)
        {
            line.erase(start, 1);
            start++;
        }
        if(line.size())
        {
            return 1;
        }
        return 0;
    }
    void My_client::trim_space(std::string& line)    
    {
        int start=0;
        std::string exclude="\a\n\t\r\v\b\f ";
        while((start=(line.find_first_of(exclude,start)))!=std::string::npos)
        {
            line.erase(start, 1);
            start++;
        }
    }
    int My_client::handle_word(std::string& search,std::string& result)
    {
        if(!not_null(search))
        {
            return 0;;
        }
        if(send_msg(search.c_str())<0)
        {
            return COLLAPSE;
        }
        recv_msg(result);
        if(result.find(RIGHTMSG)!=std::string::npos)
        {
            return 1;
        }
        else
        {
            Json::Value err_value;
            if(reader.parse(result,err_value,false))
            {
                Json::Value err_result=err_value["result"];
                std::cout<<err_value["type"].asString()<<std::endl;
                for(int index=0;index!=err_result.size();index++)
                {
                    std::cout<<err_result[index]["result"].asString()<<std::endl;
                }
                return 0;
            }
            std::cout<<"err word correct analysis  failed"<<std::endl;
            return 0;
        }
    }
    int My_client::handle_title(std::string& result)
    {
        recv_msg(result);//接收标题摘要
        if(result.find(ERRMSG)!=std::string::npos)
        {
            Json::Value err_title;
            if(reader.parse(result,err_title,false))
            {
                for(int index=0;index!=err_title.size();index++)
                {
                    std::cout<<err_title[index]["title"].asString()<<std::endl;
                    std::cout<<err_title[index]["abstract"].asString()<<std::endl;
                }
                return 0;
            }
            std::cout<<"err title message  analysis failed "<<std::endl;
            return 0;
        }
        else
        {
            return 1;
        }
    }
    void My_client::handle_content(std::string& result)
    {
        Json::Value value;
        if(reader.parse(result,value,false))
        {
            for(int index=0;index!=value.size();index++)
            {
                std::cout<<value[index]["title"].asString()<<std::endl;
                std::cout<<value[index]["abstract"].asString()<<std::endl;
            }
        }
        else
        {
            std::cout<<"handle content title abstract failed"<<std::endl;
            return;
        }
        std::string title_msg;
        while(std::cout<<"请输入页面标题"<<std::endl,std::cin>>title_msg)//按接收的标题获取标题对应的内容
        {
            trim_space(title_msg);
            Json::Value title_value;
            title_value["type"]="title";
            title_value["content"]=title_msg;
            std::string json_title=writer.write(title_value);
            send_msg(json_title.c_str());
            recv_msg(result);
            std::cout<<result;
        }
        Json::Value over_value;
        over_value["type"]="over!";
        over_value["content"]="client has been finish it's search or close port";
        std::string finish_msg=writer.write(over_value);
        send_msg(finish_msg.c_str());
        std::cout<<"client has been finish one search"<<std::endl;
    }
    void My_client::socket_init()
    {
        server_sock=socket(AF_INET,SOCK_STREAM,0);
        if(server_sock==-1)
        {
            perror("server sock create failed");
        }
        memset(&server_addr,0,sizeof(server_addr));
        server_addr.sin_family=AF_INET;
        server_addr.sin_port=htons(m_port);
        server_addr.sin_addr.s_addr=inet_addr(m_ip.c_str());
        //set_nonblock(server_sock);
    }
    int  My_client::set_nonblock(int& sockfd)
    {
        int old=fcntl(sockfd,F_GETFL);
        old|=O_NONBLOCK;
        fcntl(sockfd,F_SETFL,old);
        return 0;
    }
    void My_client::socket_error(const std::string& err_msg,const int& value)
    {
        if(value==-1)
        {
            perror(err_msg.c_str());
        }
    }
    int  My_client::send_msg(const char* msg)
    {
        int send_len=strlen(msg);
        std::string error_msg1="client send message length failed ";
        int ret_l=sendn(server_sock,(char*)&send_len,4);
        socket_error(error_msg1,ret_l);
        std::string error_msg2="client send message  failed ";
        int ret_m=sendn(server_sock,msg,send_len);
        socket_error(error_msg2,ret_m);
        return ret_m;
    }
    int My_client::sendn(const int& server_sock,const char* p,const int& send_len)
    {
        int send_sum=0;
        int send_ret=0;
        while(send_sum<send_len)
        {
            send_ret=send(server_sock,p+send_sum,send_len-send_sum,0);
            if(send_ret<0)
            {
                if(errno==EAGAIN)
                {
                    continue;
                }
                return -1;
            }
            send_sum+=send_ret;
        }
        return send_sum;
    }
    int My_client::socket_connect()
    {
        int connect_ret=connect(server_sock,(struct sockaddr*)&server_addr,sizeof(server_addr));
        std::string error_msg="server connect failed";
        socket_error(error_msg,connect_ret);
        std::cout<<"connect server"<<inet_ntoa(server_addr.sin_addr)<<":"<<ntohs(server_addr.sin_port)<<")"<<std::endl;
    }
    int  My_client::recv_msg(std::string& result)
    {
        result.clear();
        memset(message,0,RECV_SIZE);
        int recv_len=0;
        std::string error_msg1="server recv message length failed ";
        int ret_l=recvn(server_sock,(char*)&recv_len,4);
        socket_error(error_msg1,ret_l);
        std::string error_msg2="server recv message  failed ";
        int ret_m=recvn(server_sock,message,recv_len);
        socket_error(error_msg2,ret_m);
        result.assign(message);
        return ret_m;
    }
    int My_client::recvn(const int& server_sock,char* p,const int& recv_len)
    {
        int recv_sum=0;
        int recv_ret=0;
        while(recv_sum<recv_len)
        {
            recv_ret=recv(server_sock,p+recv_sum,recv_len-recv_sum,0);
            if(recv_ret<0)
            {
                if(errno==EAGAIN)
                {
                    continue;
                }
                return -1;
            }
            recv_sum+=recv_ret;
        }
        return recv_sum;
    }
    void My_client::handle_page()
    {
        while(err_search(result),std::cout<<"请输入查询词"<<std::endl,std::cin>>search)
        {
            int feedback=handle_word(search,result);
            if(feedback==COLLAPSE)
            {
                return;
            }
            if(!feedback)
            {
                continue;
            }
            if(handle_title(result))
            {
                break;
            }
            search.clear();
        }
        handle_content(result);
    }
}
