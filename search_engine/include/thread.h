#ifndef __THREAD_H
#define __THREAD_H
#include "queue.h"
#include<iostream>
#include<pthread.h>
#include"conf.h"
namespace THREAD_POOL
{
    class CThread_pool;
}
namespace THREAD
{
    class CThread_RUN
    {
        public:
            virtual void run()=0;
            virtual void set_cache(hash_record* m_cache)=0;
            virtual void set_page_cache(hash_record* m_page_cache)=0;
            virtual void set_title_cache(hash_record* m_title_cache)=0;
            virtual void set_log(LOG_SYSTEM::Log_system* log)=0;
    };
    class CThread
    {
        public:
            CThread():m_cache(HASH_BASE),m_page_cache(HASH_BASE){}
            CThread(LOG_SYSTEM::Log_system* log):m_log(log),m_cache(HASH_BASE),m_page_cache(HASH_BASE){}
            void start(CThread_RUN* arg);
            hash_record* get_record();
            hash_record* get_page_cache();
            hash_record* get_title_cache();
            void set_log(LOG_SYSTEM::Log_system* log);
        private:
            pthread_t m_tid;
            hash_record m_cache;
            hash_record m_page_cache;
            hash_record m_title_cache;
            LOG_SYSTEM::Log_system* m_log;
            static void* thread_func(void* obj);
    };
}
#endif
