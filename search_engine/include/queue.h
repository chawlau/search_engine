#ifndef __QUE_H
#define __QUE_H
#include"mutex.h"
#include"cond.h"
#include<queue>
#include<set>
#include<iostream>
#include"log.h" 
#include"search.h"
namespace QUEUE
{
    class CQueue
    {
        public:
            class CTask
            {
                public:
                    virtual void excute(hash_record* cache,hash_record* m_page_cache,hash_record* title_cache,LOG_SYSTEM::Log_system* log)=0;
            };
            class CPtask
            {
                public:
                    CPtask(CTask* ptr=NULL):m_ptr(ptr){}
                    CTask* operator->();
                    CTask& operator*();
                private:
                    CTask* m_ptr;
            };
            CQueue(int capacity=10):m_capacity(capacity),m_mutex(),m_pro(m_mutex),m_con(m_mutex),m_flag(true){}
            bool consume(CPtask* task);
            bool produce(CPtask task);
            ~CQueue(){}
        private:
            CQueue(const CQueue& obj);
            CQueue& operator=(const CQueue& obj);
            MUTEX::CMutex m_mutex;
            COND::CCond m_pro,m_con;
            bool m_flag;
            int m_capacity;
            std::queue<CPtask> m_que;
    };
}
#endif
