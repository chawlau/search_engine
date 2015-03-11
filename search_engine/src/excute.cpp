#include"excute.h"
int not_null(const std::string& word)
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
namespace EXCUTE
{
    CTask_excute::CTask_excute(WORD_CORRECT::CWord_correct& correct,PAGE_RANK::Page_rank* rank,const int& fd_client):m_correct(correct),m_fd_client(fd_client)
    {
        m_rank=rank; 
    }
    int CTask_excute::word_correct(hash_record* m_cache,std::string& word_result,LOG_SYSTEM::Log_system* m_log,std::string& m_search)
    {
        m_search.clear();
        m_search.assign(message);
        if(!not_null(m_search))
        {
            return 0;
        }
        std::stringstream s1;
        s1<<"work thread ID: "<<pthread_self()<<" recv word is  "<<m_search<<std::endl;
        m_log->notice(s1.str());
        if((*m_cache).count(m_search)>0)//从词汇缓存中查找历史纪录
        {
            m_log->notice("run thread's word_cache has result,return answer to client directly");
            word_result=(*m_cache)[m_search];
        }
        else
        {
            m_log->notice("run thread's page_cache no result,go to hash_map index of dic for answer");
            m_correct(m_search,word_result);
            if(not_null(m_search)&&not_null(word_result))
            {
                std::pair<std::string,std::string> apair=make_pair(m_search,word_result);
                (*m_cache).insert(apair);
            }
        }
        //std::cout<<word_result<<std::endl;
        send_msg(word_result.c_str(),m_log);
        if(word_result.find("输入正确")!=std::string::npos)
        {
            return 1;
        }
        return 0;
    }
    int CTask_excute::handle_title(hash_record* m_page_cache,hash_record* m_title_cache,LOG_SYSTEM::Log_system* m_log,int& has_result,std::string& m_search,std::string& page_result)
    {
        if((*m_page_cache).count(m_search)>0)//从网页纪录中查询历史纪录
        {
            m_log->notice("run thread's word_cache has result,return answer to client directly");
            page_result=(*m_page_cache)[m_search]; 
            send_msg(page_result.c_str(),m_log);
            if(page_result.find(ERRMSG)!=std::string::npos)
            {
                return 0;
            }
            return 1;
        }
        else//没有历史记录，从网页库中查找，并且返回记录
        {
            hash_record title_content;
            m_log->notice("run thread's page_cache no result,go to word_page_rindex  for answer");
            has_result=(*m_rank)(m_search,page_result,title_content);//获取标题摘要，标题内容
            send_msg(page_result.c_str(),m_log);//发送标题摘要
            if(not_null(m_search)&&not_null(page_result))
            {
                std::pair<std::string,std::string> apair=make_pair(m_search,page_result);
                (*m_page_cache).insert(apair);
            }
            m_log->notice("put title_abstract in thread's page_cache");
            piter title_iter=title_content.begin();//将标题内容同步各自的线程
            for(title_iter;title_iter!=title_content.end();title_iter++)
            {
                if(not_null(title_iter->first)&&not_null(title_iter->second))
                {
                    std::pair<std::string,std::string> pair2=make_pair(title_iter->first,title_iter->second);
                    (*m_title_cache).insert(pair2);
                }
            }
            m_log->notice("put title_content in thread's title_cache");
            if(has_result)
            {
                return 1;
            }
            return 0;
        }
    }
    void CTask_excute::handle_title_content(hash_record* m_title_cache,LOG_SYSTEM::Log_system* m_log)
    {
        Json::Reader reader;
        Json::Value value;
        value["type"]="init";
        value["content"]="init";
        std::string title_order;
        while(value["type"].asString()!="over!")//根据客户端发送的标题返回标题对应的结果给客户端
        {
            title_order.clear();
            recv_msg(m_log);
            title_order.assign(message);
            Json::Value title_value;
            reader.parse(title_order,title_value);
            value["type"]=title_value["type"].asString();
            value["content"]=title_value["content"].asString();
            if(title_value["type"]=="title")
            {
                std::string title=title_value["content"].asString();
                if((*m_title_cache).count(title)>0)
                {
                    send_msg((*m_title_cache)[title].c_str(),m_log);
                }
                else
                {
                    std::string page_content=ERRMSG;
                    send_msg(page_content.c_str(),m_log);
                }
            }
            else
            {
                break;
            }
        }
        std::stringstream s1;
        s1<<"work thread ID: "<<pthread_self()<<"client has been finish it's search_task or shutdown!"<<std::endl;
        m_log->notice(s1.str());
    }
    void CTask_excute::excute(hash_record* m_cache,hash_record* m_page_cache,hash_record* m_title_cache,LOG_SYSTEM::Log_system* m_log)
    {
        bzero(message,MAXSIZE);
        std::string m_search;
        std::string word_result;
        int has_result=0;
        /*-------------------只有查询词正确而且网页库中有相应的查询结果才能退出循环-----*/
        while(!(word_result.find("输入正确")!=std::string::npos&&has_result!=0))
        {
            word_result.clear();
            if(!recv_msg(m_log))
            {
                return ;
            }
            /*--------------处理查询词纠错问题----------------------*/
            if(!word_correct(m_cache,word_result,m_log,m_search))
            {
                continue;
            }
            std::string page_result;
            /*----------------处理查询标题摘要结果------------------*/
            if(handle_title(m_page_cache,m_title_cache,m_log,has_result,m_search,page_result))
            {
                break;
            }
        }
        std::stringstream s1;
        s1<<"work thread ID: "<<pthread_self()<<" recv msg is right and has page result "<<std::endl;
        m_log->notice(s1.str());
        /*-------------处理标题对应的网页内容-------------------------*/
        handle_title_content(m_title_cache,m_log);
        close(m_fd_client);
    }
    void CTask_excute::socket_error(const std::string& err_msg,const int& value,LOG_SYSTEM::Log_system* m_log)
    {
        if(value==-1)
        {
            m_log->error(err_msg);
            //perror(err_msg.c_str());
        }
    }
    bool  CTask_excute::send_msg(const char* content,LOG_SYSTEM::Log_system* m_log)
    {
        int send_len=strlen(content);
        std::stringstream s1;
        s1<<pthread_self()<<" server send message length failed "<<std::endl;
        int ret_l=sendn(m_fd_client,(char*)&send_len,4);
        socket_error(s1.str(),ret_l,m_log);
        if(ret_l<4)
        {
            return false;
        }
        std::stringstream s2;
        s2<<pthread_self()<<" server send message  failed "<<std::endl;
        int send_ret=sendn(m_fd_client,content,send_len);
        socket_error(s2.str(),send_ret,m_log);
        return send_ret==send_len;
    }
    int CTask_excute::sendn(const int& fd_client,const char* p,const int& send_len)
    {
        int send_sum=0;
        int send_ret=0;
        while(send_sum<send_len)
        {
            send_ret=send(fd_client,p+send_sum,send_len-send_sum,0);
            if(send_ret<0)
            {
                /*
                   if(errno==EINTR)
                   {
                   return -1;
                   }*/
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
    bool  CTask_excute::recv_msg(LOG_SYSTEM::Log_system* m_log)
    {
        memset(message,0,MAXSIZE);
        int recv_len=0;
        std::stringstream s1;
        s1<<pthread_self()<<" ERROR server recv message length failed "<<std::endl;
        int ret_l=recvn(m_fd_client,(char*)&recv_len,4);
        m_log->notice(s1.str());
        if(ret_l!=4)
        {
            return false;
        }
        std::stringstream s2;
        s2<<pthread_self()<<" server recv message  failed "<<std::endl;
        int recv_ret=recvn(m_fd_client,message,recv_len);
        socket_error(s2.str(),recv_ret,m_log);
        return recv_ret==recv_len;
    }
    int CTask_excute::recvn(const int& fd_client,char* p,const int& recv_len)
    {
        int recv_sum=0;
        int recv_ret=0;
        while(recv_sum<recv_len)
        {
            recv_ret=recv(fd_client,p+recv_sum,recv_len-recv_sum,0);
            if(recv_ret<0)
            {
                /*
                   if(errno==EINTR)
                   {
                   return -1;
                   }*/
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
    int  CTask_excute::set_nonblock(int& sockfd)
    {
        int old=fcntl(sockfd,F_GETFL);
        old|=O_NONBLOCK;
        fcntl(sockfd,F_SETFL,old);
        return 0;
    }
    CRun::CRun(QUEUE::CQueue* pq):m_pq(pq){}
    void CRun::run()
    {
        while(1)
        {
            QUEUE::CQueue::CPtask task;
            if(m_pq->consume(&task)==false)
            {
                m_log->error("consume task from queue false");
            }
            std::stringstream s1;
            s1<<"work thread ID: "<<pthread_self()<<" handle the search task "<<std::endl;
            m_log->notice(s1.str());
            task->excute(m_cache,m_page_cache,m_title_cache,m_log);
            std::stringstream s2;
            s2<<"work thread ID: "<<pthread_self()<<" finish the search task"<<std::endl;
            m_log->notice(s2.str());
            sleep(rand()%3+1);
        }
    }
    void CRun::set_cache(hash_record* cache)
    {
        m_log->notice("set run thread's hash_map cache");
        m_cache=cache;
    }
    void CRun::set_page_cache(hash_record* page_cache)
    {
        m_log->notice("set run thread's page_cache");
        m_page_cache=page_cache;
    }
    void CRun::set_title_cache(hash_record* title_cache)
    {
        m_log->notice("set run thread's page_cache");
        m_title_cache=title_cache;
    }
    void CRun::set_log(LOG_SYSTEM::Log_system* log)
    {
        m_log=log;
        m_log->notice("set run thread log_system");
    }
    Cache::Cache(THREADPOOL::CThread_pool& pool):m_conf(pool.m_conf),m_thread_vector(pool.get_thread_vector()){}
    void Cache::read_disk_cache()
    {
        std::fstream fin(m_conf.config["my_cache"].c_str());
        const char* cache_path=m_conf.config["my_cache"].c_str();
        Json::Value value;
        Json::Reader reader;
        Json::FastWriter writer;
        if(reader.parse(fin,value,false))
        {
            for(int index_i=0;index_i!=value.size();index_i++)
            {
                std::string search=value[index_i]["search"].asString();
                Json::Value r_value=value[index_i]["result"];
                std::string result=writer.write(r_value);
                std::pair<std::string,std::string> apair=make_pair(search,result);
                (*m_cache).insert(apair);
            }
            m_log->notice("disk word_cache read is over!");
            return;
        }
        m_log->notice("disk word_cache read is failed,try again!");
    }
    void Cache::write_disk_cache()
    {
        std::ofstream fout(m_conf.config["my_cache"].c_str());
        Json::Value value;
        Json::FastWriter writer;
        Json::Reader reader;
        piter iter=(*m_cache).begin();
        for(iter;iter!=(*m_cache).end();iter++)
        {
            if(!not_null(iter->first)||!not_null(iter->second))
            {
                continue;
            }
            Json::Value w_value;
            Json::Value r_value;;
            w_value["search"]=iter->first;
            if(reader.parse(iter->second,r_value,false))
            {
                w_value["result"]=r_value;
                value.append(w_value);
            }
        }
        std::string jsonfile=writer.write(value);
        fout<<jsonfile;
        m_log->notice("disk word_cache write is over!");
        fout.close();
    }
    void Cache::run()
    {
        read_disk_cache();
        while(1)
        {
            m_log->notice("word_cache_thread synchronize start!");
            std::vector<THREAD::CThread>::iterator iter=m_thread_vector.begin();
            for(iter;iter!=m_thread_vector.end();iter++)//遍历每个线程的词汇,同步缓存历史纪录
            {
                piter iter1=(*(iter->get_record())).begin();
                for(iter1;iter1!=(*(iter->get_record())).end();iter1++)
                {
                    std::pair<std::string,std::string> apair=make_pair(iter1->first,iter1->second);
                    (*m_cache).insert(apair);
                }
            }
            m_log->notice("word_cache thread syschronize thread_pool's hash_map");
            for(iter;iter!=m_thread_vector.end();iter++)
            {
                hash_record temp=(*m_cache);
                temp.swap(*(iter->get_record()));
                temp.clear();
            }
            m_log->notice("word_cache thread syschronize result back to thread_pool");
            write_disk_cache();
            m_log->notice("word_cache_thread synchronize over!");
            sleep(10);
        }
    }
    void Cache::set_cache(hash_record* cache)
    {
        m_cache=cache;
        m_log->notice("set cache thread hash_map cache");
    }
    void Cache::set_page_cache(hash_record* page_cache)
    {
    }
    void Cache::set_title_cache(hash_record* title_cache)
    {
    }
    void Cache::set_log(LOG_SYSTEM::Log_system* log)
    {
        m_log=log;
        m_log->notice("set cache thread log_system ");
    }
    Page_cache::Page_cache(THREADPOOL::CThread_pool& pool):m_conf(pool.m_conf),m_thread_vector(pool.get_thread_vector()){}
    void Page_cache::read_disk_page_cache()
    {
        std::fstream fin(m_conf.config["my_page_cache"].c_str());
        const char* page_cache_path=m_conf.config["my_page_cache"].c_str();
        Json::FastWriter writer;
        Json::Reader reader;
        Json::Value jsvalue;
        if(reader.parse(fin,jsvalue,false))
        {
            for(int index=0;index!=jsvalue.size();index++)
            {
                std::string search=jsvalue[index]["search"].asString();
                Json::Value r_value=jsvalue[index]["result"];
                std::string result=writer.write(r_value);
                std::pair<std::string,std::string> apair=make_pair(search,result);
                (*m_page_cache).insert(apair);
            }
            m_log->notice("disk page_cache read is over!");
            return ;
        }
        m_log->notice("disk page_cache read is failed!");
    }
    void Page_cache::write_disk_page_cache()
    {
        std::ofstream fout(m_conf.config["my_page_cache"].c_str());
        Json::FastWriter writer;
        Json::Reader reader;
        Json::Value value;
        piter iter=(*m_page_cache).begin();
        for(iter;iter!=(*m_page_cache).end();iter++)
        {
            if(!not_null(iter->first)||!not_null(iter->second))
            {
                continue;
            }
            Json::Value t_value;
            Json::Value a_value;
            t_value["search"]=iter->first;
            if(reader.parse(iter->second,a_value,false))
            {
                t_value["result"]=a_value;
                value.append(t_value);
            }
        }
        std::string jsonfile=writer.write(value);
        fout<<jsonfile;
        m_log->notice("disk page_cache write is over!");
        fout.close();
    }
    void Page_cache::run()
    {
        read_disk_page_cache();
        while(1)
        {
            m_log->notice("page_cache synchronize start!");
            std::vector<THREAD::CThread>::iterator iter=m_thread_vector.begin();
            for(iter;iter!=m_thread_vector.end();iter++)//遍历每个线程的page_cache,同步缓存历史页面纪录
            {
                piter iter1=(*(iter->get_page_cache())).begin();
                for(iter1;iter1!=(*(iter->get_page_cache())).end();iter1++)
                {
                    if(not_null(iter1->first)&&not_null(iter1->second))
                    {
                        std::pair<std::string,std::string> apair=make_pair(iter1->first,iter1->second);
                        (*m_page_cache).insert(apair);
                    }
                }
            }
            m_log->notice("page_cache thread syschronize thread_pool's page_cache");
            iter=m_thread_vector.begin();//将同步后的page_cache同步各个线程
            for(iter;iter!=m_thread_vector.end();iter++)
            {
                hash_record temp=(*m_page_cache);
                temp.swap(*(iter->get_page_cache()));
                temp.clear();
            }
            m_log->notice("page_cache thread syschronize page_cache back to thread_pool");
            write_disk_page_cache();
            m_log->notice("page_cache synchronize over!");
            iter=m_thread_vector.begin();
            for(iter;iter!=m_thread_vector.end();iter++)//遍历每个线程的title_cache,同步缓存标题内容纪录
            {
                piter iter1=(*(iter->get_title_cache())).begin();
                for(iter1;iter1!=(*(iter->get_title_cache())).end();iter1++)
                {
                    if(not_null(iter1->first)&&(not_null(iter1->second)))
                    {
                        std::pair<std::string,std::string> apair=make_pair(iter1->first,iter1->second);
                        (*m_title_cache).insert(apair);
                    }
                }
            }
            m_log->notice("page_cache thread syschronize thread_pool's title_cache");
            iter=m_thread_vector.begin();//将同步后的title_cache同步各个线程
            for(iter;iter!=m_thread_vector.end();iter++)
            {
                hash_record temp=(*m_title_cache);
                temp.swap(*(iter->get_title_cache()));
                temp.clear();
            }
            m_log->notice("page_cache thread syschronize title_cache back to thread_pool");
            m_log->notice("title_cache synchronize over!");
            sleep(10);
        }
    }
    void Page_cache::set_cache(hash_record* cache)
    {
    }
    void Page_cache::set_page_cache(hash_record* page_cache)
    {
        m_log->notice("set page_cache thread's page_cache");
        m_page_cache=page_cache;
    }
    void Page_cache::set_title_cache(hash_record* title_cache)
    {
        m_log->notice("set page_cache thread's page_cache");
        m_title_cache=title_cache;
    }
    void Page_cache::set_log(LOG_SYSTEM::Log_system* log)
    {
        m_log=log;
        m_log->notice("set page_cache thread log_system ");
    }
}
