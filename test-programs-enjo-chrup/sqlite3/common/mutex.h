#ifndef __MUTEX_H_
#define __MUTEX_H_

#include <pthread.h>

class Mutex
{
    pthread_mutex_t m_mMutex;

  public:

    Mutex() { pthread_mutex_init(&this->m_mMutex, NULL); }
    virtual ~Mutex() { pthread_mutex_destroy(&this->m_mMutex); }
    void Lock() { pthread_mutex_lock(&this->m_mMutex); }
    void Unlock() { pthread_mutex_unlock(&this->m_mMutex); }

    operator pthread_mutex_t() { return this->m_mMutex; }
    operator pthread_mutex_t*() { return &this->m_mMutex; }
};

#endif // __MUTEX_H_
