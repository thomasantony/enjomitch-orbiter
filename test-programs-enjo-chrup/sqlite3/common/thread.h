#ifndef __THREAD_H_
#define __THREAD_H_

#include <pthread.h>

#include "mutex.h"
#include "event.h"
#include "threadex.h"

class Thread
{
  public:

    class Id
    {
        pthread_t m_idThread;

      public:

        Id() {}
        Id(const pthread_t& idThread) : m_idThread(idThread) {}

        operator pthread_t() { return this->m_idThread; }
        operator pthread_t*() { return &this->m_idThread; }
    };
    
    enum State 
    {
      STOPPED = 0,
      RUNNING = 1,
      ERROR_OCCURED = 2,
    };

  private:

    static void* Runner(void* pParam);

    Id                m_idThread;
    pthread_attr_t    m_attrThread;
    Event             m_evRunning;
    Mutex             m_muxState;
    State             m_stateThread;

  protected:

    Thread();

    virtual void OnRun() = 0;

  public:

    virtual ~Thread() throw();

    const Id& GetId() { return this->m_idThread; }
  
    State GetState();
    void SetRunningState();
    void SetStoppedState();
    void SetErrorState();

    void Run();
    void Stop();	

};

#endif // __THREAD_H_
