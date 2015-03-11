#ifndef __FUNC_H
#define __FUNC_H
#include"thread_pool.h"
#include"page_rank.h"
#include<sstream>
#include"conf.h"
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#define MAXSIZE 1024
#define ERRMSG "你所查询的页面不存在或者已经被删除!"
namespace EXCUTE
{
    class CTask_excute:public QUEUE::CQueue::CTask
    {
        public:
            CTask_excute(WORD_CORRECT::CWord_correct& correct,PAGE_RANK::Page_rank* rank,const int& fd_client);
            int word_correct(hash_record* m_cache,std::string& word_result,LOG_SYSTEM::Log_system* m_log,std::string& m_search);
            int handle_title(hash_record* m_page_cache,hash_record* m_title_cache,LOG_SYSTEM::Log_system* m_log,int& has_result,std::string& m_search,std::string& page_result);
            void handle_title_content(hash_record* m_title_cache,LOG_SYSTEM::Log_system* m_log);
            void excute(hash_record* m_cache,hash_record* m_page_cache,hash_record* m_title_cache,LOG_SYSTEM::Log_system* log);
            bool send_msg(const char* msg,LOG_SYSTEM::Log_system* log);
            bool recv_msg(LOG_SYSTEM::Log_system* log);
            void socket_error(const std::string& message,const int& value,LOG_SYSTEM::Log_system* log);
            int sendn(const int& fd_client,const char* p,const int& send_len);
            int recvn(const int& fd_client,char* p,const int& send_len);
            int set_nonblock(int& sockfd);
        private:
            WORD_CORRECT::CWord_correct& m_correct;
            PAGE_RANK::Page_rank* m_rank;
            const int& m_fd_client;
            char message[MAXSIZE];
    };
    class CRun:public THREAD::CThread_RUN
    {
        public:
            CRun(QUEUE::CQueue* pq);
            void run();
            void set_log(LOG_SYSTEM::Log_system* log);
            void set_cache(hash_record* cache);//词汇设置缓存
            void set_page_cache(hash_record* page_cache);//设置查询网页结果缓存
            void set_title_cache(hash_record* title_cache);//设置查询标题页面缓存
        private:
            QUEUE::CQueue* m_pq;
            hash_record* m_cache;
            hash_record* m_page_cache;            
            hash_record* m_title_cache;
            LOG_SYSTEM::Log_system* m_log;
    };
    class Cache:public THREAD::CThread_RUN
    {
        public:
            Cache(THREADPOOL::CThread_pool& pool);
            void run();
            void set_cache(hash_record* cache);
            void set_page_cache(hash_record* cache);
            void set_title_cache(hash_record* cache);
            void set_log(LOG_SYSTEM::Log_system* log);
            void cache_out();
            void read_disk_cache();//读取磁盘缓存纪录
            void write_disk_cache();//更新磁盘缓存纪录
        private:
            hash_record* m_cache;
            MY_CONF::CMyconf& m_conf;
            std::vector<THREAD::CThread>& m_thread_vector;
            LOG_SYSTEM::Log_system* m_log; 
    };
    class Page_cache:public THREAD::CThread_RUN
    {
        public:
            Page_cache(THREADPOOL::CThread_pool& pool);
            void run();
            void set_page_cache(hash_record* cache);
            void set_title_cache(hash_record* cache);
            void set_cache(hash_record* cache);
            void set_log(LOG_SYSTEM::Log_system* log);
            void cache_out();
            void read_disk_page_cache();//读取磁盘缓存纪录
            void write_disk_page_cache();//更新磁盘缓存纪录
        private:
            hash_record* m_page_cache;
            hash_record* m_title_cache;
            MY_CONF::CMyconf& m_conf;
            std::vector<THREAD::CThread>& m_thread_vector;
            LOG_SYSTEM::Log_system* m_log; 
    };
}
#endif
