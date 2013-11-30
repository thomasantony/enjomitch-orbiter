#ifndef __EVENT_H_
#define __EVENT_H_

#include <pthread.h>

#include "mutex.h"
#include "cond.h"

//@author Justin Olbrantz (Quantam)
class Event // POSIX version of event: condition variable (fast?)
{
    bool            m_bAutoReset; // Constant    
    Cond            m_condEvent;
    Mutex           m_muxEvent;
    bool            m_bSet; // Set or clear // Protected by m_mutex
    unsigned int    m_nWaitingThreads; // Number of threads waiting on the event

    bool InnerWait();

  public:
  
    Event(bool bAutoReset, bool bSet) : m_bAutoReset(bAutoReset), m_bSet(bSet), m_nWaitingThreads(0) {}
    virtual ~Event() {}

    void Set();

    inline void Reset()
    {
      this->m_muxEvent.Lock();
      this->m_bSet = false; // Ding
      this->m_muxEvent.Lock();
    }

    inline void Wait()
    {
      this->m_muxEvent.Lock();
      InnerWait();
      this->m_muxEvent.Unlock();
    }

    inline bool IsSet()
    {
      bool bIsSet;
      this->m_muxEvent.Lock();
      bIsSet = this->m_bSet;
      this->m_muxEvent.Unlock();
      return bIsSet;
    }
};

#endif // __EVENT_H_
