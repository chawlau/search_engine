#ifndef _MUTEX_H
#define _MUTEX_H
#include<unistd.h>
#include<string>
#include<stdexcept>
#include<pthread.h>
namespace COND
{
    class CCond;
}
namespace MUTEX
{
    class CMutex
    {
        public:
            friend class COND::CCond;
            CMutex()
            {
                if(pthread_mutex_init(&m_mutex,NULL)!=0)
                {
                    throw std::runtime_error("mutex init");
                }
            }
            void lock()
            {
                pthread_mutex_lock(&m_mutex);
            }
            void unlock()
            {
                pthread_mutex_unlock(&m_mutex);
            }
            ~CMutex()
            {
                pthread_mutex_destroy(&m_mutex);
            }
        private:
            CMutex& operator=(const CMutex onbj);
            CMutex(const CMutex& obj);
            pthread_mutex_t m_mutex;
    };
}
#endif

