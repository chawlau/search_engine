#include "query_handle.h"
namespace QUERY_HANDLE
{
    Query_handle::Query_handle(MY_CONF::CMyconf& conf,LOG_SYSTEM::Log_system* log):m_correct(conf.config["my_dic"].c_str()),m_pool(conf,log),m_run(&m_pool.m_que),m_cache_run(m_pool),m_page_cache_run(m_pool)
    {

        m_pool.on(&m_run,&m_cache_run,&m_page_cache_run);
        m_rank=PAGE_RANK::Page_rank::getInstance(conf);
        m_rank->init();
    }
    void Query_handle::operator()(const int& fd_client)
    {
        EXCUTE::CTask_excute* ptr=new EXCUTE::CTask_excute(m_correct,m_rank,fd_client);
        m_pool.m_que.produce(ptr);
    }
}

