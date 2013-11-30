#include "event.h"

bool Event::InnerWait()
{
  if (!this->m_bSet) {
    this->m_nWaitingThreads++;
    int nRetVal;
    do {
      nRetVal = pthread_cond_wait(this->m_condEvent, this->m_muxEvent);
    } while (!this->m_bSet /*&& nRetVal != ETIMEDOUT*/); // Loop until it gets set or the timeout expires
    this->m_nWaitingThreads--;
  }
  // Did the event get set?
  bool bSuccess = this->m_bSet;
  // If the event is set and it's an auto-reset event, reset it now that we're awake
  if (this->m_bSet && this->m_bAutoReset) {
    this->m_bSet = false;
  }
  return bSuccess;
}

void Event::Set()
{
  this->m_muxEvent.Lock();
  if (!this->m_bSet) { // If it's already set, do nothing
    this->m_bSet = true; // Set the event
  }
  // Check if there are any waiters, and release them appropriately
  if (this->m_nWaitingThreads) {
    if (this->m_bAutoReset) {
      this->m_condEvent.Signal(); // Release one thread
    } else {
      this->m_condEvent.Broadcast(); // Release all threads
    }
  }
  this->m_muxEvent.Unlock();
}

