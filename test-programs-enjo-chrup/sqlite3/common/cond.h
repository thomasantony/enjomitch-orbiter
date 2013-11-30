#ifndef __COND_H_
#define __COND_H_

#include <pthread.h>

class Cond
{
    pthread_cond_t  m_cCondition; 

  public:

    Cond() { pthread_cond_init(&this->m_cCondition, NULL); }
    virtual ~Cond() { pthread_cond_destroy(&this->m_cCondition); }
    void Signal() { pthread_cond_signal(&this->m_cCondition); }
    void Broadcast() { pthread_cond_broadcast(&this->m_cCondition); }

    operator pthread_cond_t() { return this->m_cCondition; }
    operator pthread_cond_t*() { return &this->m_cCondition; }
};

#endif // __COND_H_
