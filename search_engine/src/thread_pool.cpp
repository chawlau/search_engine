#include "thread_pool.h"
namespace  THREADPOOL
{
    CThread_pool::CThread_pool(MY_CONF::CMyconf& conf,LOG_SYSTEM::Log_system* log):m_conf(conf),m_thread_pool(THREAD_NUM),m_que(QUEUE_CAPACITY),cache_thread(log),page_cache_thread(log)
    {
        std::vector<THREAD::CThread>::iterator iter=m_thread_pool.begin();
        for(iter;iter!=m_thread_pool.end();iter++)
        {
            iter->set_log(log);
        }
    }
    void  CThread_pool::on(THREAD::CThread_RUN* thread_run,THREAD::CThread_RUN* thread_cache,THREAD::CThread_RUN* thread_page_cache)
    {
        if(m_flag==false)
        {
            m_flag=true;
        }
        cache_thread.start(thread_cache);
        page_cache_thread.start(thread_page_cache);
        std::vector<THREAD::CThread>::iterator iter=m_thread_pool.begin();
        for(iter;iter!=m_thread_pool.end();iter++)
        {
            iter->start(thread_run);
        }
    }
    void  CThread_pool::off()
    {
        if(m_flag==true)
        {
            m_flag=false;
        }
    }
    std::vector<THREAD::CThread>&  CThread_pool::get_thread_vector()
    {
        return m_thread_pool;
    }
}
