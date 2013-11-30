#include "thread.h"
  
Thread::Thread() : m_evRunning(true, false), m_stateThread(STOPPED)
{
  // Initialize and set thread detached attribute
  pthread_attr_init(&this->m_attrThread);
  pthread_attr_setdetachstate(&this->m_attrThread, PTHREAD_CREATE_DETACHED);
}

Thread::~Thread() throw() 
{ 
  // Stop thread and free attribute
  this->Stop(); 
  pthread_attr_destroy(&this->m_attrThread);
}

void Thread::Run() 
{ 
  int nError = pthread_create(this->m_idThread, &this->m_attrThread, &Thread::Runner, (void *)this); 
  if (nError) {
    throw ThreadException("Thread::Run() -> pthread_create");
  } else {
    this->m_evRunning.Wait();
  }
}

void Thread::Stop() 
{
  if (this->GetState() != STOPPED) {
    int nError = pthread_cancel(this->m_idThread);
    if (nError) {
      throw ThreadException("Thread::Stop() -> pthread_cancel");
    }
  }
}

inline 
State Thread::GetState() throw()
{
  State stateThread;
  this->m_muxState.Lock();
  stateThread = this->m_stateThread;
  this->m_muxState.Unlock();
  return stateThread;
}

inline 
void Thread::SetRunningState() throw()
{
  this->m_muxState.Lock();
  this->m_stateThread = Thread::RUNNING;
  this->m_muxState.Unlock();
}

inline 
void Thread::SetStoppedState() throw()
{
  this->m_muxState.Lock();
  this->m_stateThread = Thread::STOPPED;
  this->m_muxState.Unlock();
}

inline 
void SetErrorState() throw()
{
  this->m_muxState.Lock();
  this->m_stateThread = Thread::ERROR_OCCURED;
  this->m_muxState.Unlock();
}

void Destroy(Thread* pThread)
{
#ifdef _DEBUG
  std::cout << "Thread deleted: " << pThread << std::endl;
#endif

  delete pThread;
}

void* Thread::Runner(void* pParam) 
{
  Thread* _this = (Thread*)pParam;
  _this->SetRunningState();
  _this->m_evRunning.Set();
  try {
    _this->OnRun();
  } catch(ThreadException& exThread) {
    _this->SetErrorState();
    return 1;
  } catch(...) {
    _this->SetErrorState();
    return 1;
  }
  _this->SetStoppedState();
  return NULL;
}
