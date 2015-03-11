#ifndef __THREAD_POOL
#define __THREAD_POOL
#include "thread.h"
#include <vector>
#define THREAD_NUM 10
#define QUEUE_CAPACITY 10
namespace  THREADPOOL
{
    class CThread_pool
    {
        public:  
        CThread_pool(MY_CONF::CMyconf& conf,LOG_SYSTEM::Log_system* log);
        void on(THREAD::CThread_RUN* thread_run,THREAD::CThread_RUN* thread_cache,THREAD::CThread_RUN* thread_page_cache);
        void off();
        std::vector<THREAD::CThread>& get_thread_vector();
        ~CThread_pool(){}
        QUEUE::CQueue m_que;
        MY_CONF::CMyconf& m_conf;
        private:
        bool m_flag;
        std::vector<THREAD::CThread> m_thread_pool;
        THREAD::CThread cache_thread;
        THREAD::CThread page_cache_thread;
        CThread_pool(const CThread_pool& obj);
        CThread_pool& operator=(const CThread_pool& obj);
    };
}
#endif
