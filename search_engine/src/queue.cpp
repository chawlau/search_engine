#include"queue.h"
namespace QUEUE
{
    CQueue::CTask*  CQueue::CPtask::operator->()
    {
        return m_ptr;
    }
    CQueue::CTask& CQueue::CPtask::operator*()
    {
        return *m_ptr; 
    }
    bool CQueue::consume(CPtask* task)
    {
        m_mutex.lock();
        while(m_flag&&m_que.empty())
        {
            m_con.wait();
        }
        if(!m_flag)
        {
            m_con.notifyall();
            return false;
        }
        *task=m_que.front();
        m_que.pop();
        m_pro.notifyall();
        m_con.notifyall();
        m_mutex.unlock();
        return true;
    }
    bool CQueue::produce(CPtask task)
    {
        m_mutex.lock();
        while(m_flag&&m_que.size()>=m_capacity)
        {
            m_pro.wait();
        }
        if(!m_flag)
        {
            m_pro.notifyall();
            return false;
        }
        m_que.push(task);
        m_con.notifyall();
        m_pro.notifyall();
        m_mutex.unlock();
        return true;
    }
}
