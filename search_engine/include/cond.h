#ifndef _COND_H
#define _COND_H
#include <pthread.h>
#include "mutex.h"
namespace COND
{
    class CCond
    {
        public:
            CCond(MUTEX::CMutex& mutex):m_mutex(mutex)
        {
            if(pthread_cond_init(&m_cond,NULL)!=0)
            {
                throw std::runtime_error("cond init");
            }
        }
            void wait()
            {
               pthread_cond_wait(&m_cond,&m_mutex.m_mutex);
            }
            void notify()
            {
                pthread_cond_signal(&m_cond);
            }
            void notifyall()
            {
                pthread_cond_broadcast(&m_cond);
            }
            ~CCond()
            {
                pthread_cond_destroy(&m_cond);
            }
        private:
            MUTEX::CMutex& m_mutex;
            pthread_cond_t m_cond;
            CCond(const CCond& obj);
            CCond& operator=(const CCond& obj);
    };
}
#endif
