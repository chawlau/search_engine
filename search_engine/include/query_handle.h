#ifndef __QUERY_HANDLE_H
#define __QUERY_HANDLE_H
#include "excute.h"
namespace QUERY_HANDLE
{
    class Query_handle
    {
        public:
            Query_handle(MY_CONF::CMyconf& conf,LOG_SYSTEM::Log_system* log);
            void operator()(const int& fd_client);
        private:
            WORD_CORRECT::CWord_correct m_correct;
            THREADPOOL::CThread_pool m_pool;
            EXCUTE::CRun m_run;
            EXCUTE::Cache m_cache_run;
            EXCUTE::Page_cache m_page_cache_run;
            PAGE_RANK::Page_rank* m_rank;
    };
}
#endif

