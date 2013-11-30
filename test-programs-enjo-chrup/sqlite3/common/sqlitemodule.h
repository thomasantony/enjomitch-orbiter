#ifndef _SQLITEMODULE_H_
#define _SQLITEMODULE_H_

#include <windows.h>
#include <iostream>

#include <string>
#include <vector>
#include <queue> 
#include <exception>
#include <winsock2.h>
#include <pthread.h>
#include <assert.h>

#include "sqlite3.h"

class SQLiteException : public std::exception
{
    std::string m_strWhat;

  public:
  
    SQLiteException(const std::string& strWhat) : m_strWhat(strWhat) {}
    virtual ~SQLiteException() throw() {}
    
    virtual const char* what() const throw() { return m_strWhat.c_str(); }
};
 
class SQLiteModule
{   
    class Mutex
    {
      pthread_mutex_t m_mMutex;

      public:

        Mutex() { pthread_mutex_init(&this->m_mMutex, NULL); }
        virtual ~Mutex() { pthread_mutex_destroy(&this->m_mMutex); }
        void Lock() { pthread_mutex_lock(&this->m_mMutex); }
        void Unlock() { pthread_mutex_unlock(&this->m_mMutex); }
    };

    //@author Justin Olbrantz (Quantam)
    class CEvent // POSIX version of event: condition variable (fast?)
    {
        bool m_bAutoReset; // Constant
        pthread_mutex_t m_mutex; // Mutex
        pthread_cond_t m_cond; // Condition variable
        
        // Protected by m_mutex
        bool m_bSet; // Set or clear
        unsigned int m_nWaitingThreads; // Number of threads waiting on the event

        // As the name implies, this must be called inside the mutex
        // Does the wait. The parameter specifies when the thread should wake up, should the event not get set before then. If this is NULL, the thread will wait indefinitely on the event. Returns whether the event got set (if not, the timeout must have expired).
        bool InnerWait()//const timespec *restrict abstime
        {
          if (!this->m_bSet) {
            this->m_nWaitingThreads++;
            int nRetVal;
            do {
              // Do the wait, either timed or indefinite
              //if (abstime) {
                //nRetVal = pthread_cond_timedwait(&this->m_cond, &this->m_mutex, abstime);
              //} else {
                nRetVal = pthread_cond_wait(&this->m_cond, &this->m_mutex);
              //}
              //assert(nRetVal == 0 || nRetVal == ETIMEDOUT);
            } while (!this->m_bSet && nRetVal != ETIMEDOUT); // Loop until it gets set or the timeout expires
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

      public:
      
        CEvent(bool bAutoReset, bool bSet)
        {
          if (pthread_mutex_init(&this->m_mutex, NULL) != 0) {
            //throw std::bad_alloc("Unable to create mutex");
          } else if (pthread_cond_init(&this->m_cond, NULL) != 0) {
            pthread_mutex_destroy(&this->m_mutex);
            //throw std::bad_alloc("Unable to create condition variable");
          }
          this->m_bAutoReset = bAutoReset;
          this->m_bSet = bSet;
          this->m_nWaitingThreads = 0;
        }

        inline ~CEvent()
        {
          pthread_cond_destroy(&this->m_cond);
          pthread_mutex_destroy(&this->m_mutex);
        }

        void Set()
        {
          pthread_mutex_lock(&this->m_mutex);
          if (!this->m_bSet) { // If it's already set, do nothing
            this->m_bSet = true; // Set the event
          }
          // Check if there are any waiters, and release them appropriately
          if (this->m_nWaitingThreads) {
            if (this->m_bAutoReset) {
              pthread_cond_signal(&this->m_cond); // Release one thread
            } else {
              pthread_cond_broadcast(&this->m_cond); // Release all threads
            }
          }
          pthread_mutex_unlock(&this->m_mutex);
        }

        inline void Reset()
        {
          pthread_mutex_lock(&this->m_mutex);
          this->m_bSet = false; // Ding
          pthread_mutex_unlock(&this->m_mutex);
        }
        /*
        bool Wait(unsigned int nTimeoutMS)
        {
          // Calculate the time to wake based on the time to sleep. I hope I understand how this is supposed to work on POSIX.
          timespec now, timeout, later;
          clock_gettime(CLOCK_REALTIME, &now);
          timeout.tv_sec = nTimeoutMS / 1000; // Seconds
          timeout.tv_nsec = (nTimeoutMS % 1000) * 1000000L; // Nanoseconds
          later.tv_sec = now.tv_sec + timeout.tv_sec;
          later.tv_nsec = now.tv_nsec + timeout.tv_nsec;
          if (later.tv_nsec >= 1000000000L) {
            later.tv_nsec -= 1000000000L;
            later.tv_sec++;
          }
          pthread_mutex_lock(&this->m_mutex);
          bool bSuccess = InnerWait(&later);
          pthread_mutex_unlock(&this->m_mutex);
          return bSuccess;
        }
        */
        inline void Wait()
        {
          pthread_mutex_lock(&this->m_mutex);
          InnerWait();//NULL);
          pthread_mutex_unlock(&this->m_mutex);
        }
    };

    typedef struct ClientParam {
      SOCKET ListenSocket;
      SOCKET RemoteSocket; 
    };

    static sqlite3*                                 dbSQL;
    static char*                                    zErrMsg;
    
    static CEvent                                   evEvent;
    static Mutex                                    mMutex;
    static pthread_t                                tServerId;
    static std::vector<std::string>                 vSQLBackBuffer;
    static bool                                     bServerIsRunning;
    static const unsigned                           nMaxConnections = 1;
    
    static Mutex                                    mClientConnectionsMutex;
    static std::queue<SQLiteModule::ClientParam *>  qClientConnections;
    static unsigned                                 nConnections;

    SQLiteModule();
    SQLiteModule(const SQLiteModule &);
    SQLiteModule& operator=(const SQLiteModule&);
        
    static int SQLiteCallback(void *arg, int argc, char **argv, char **azColName)
    {
      int i;
      std::string strResult;
      for (i = 0; i < argc; ++i) {
        if (i < (argc - 1)) {
          strResult = std::string(argv[i] ? argv[i] : "NULL");
        } else {
          strResult = std::string(argv[i] ? argv[i] : "");  // we don't need a newline
        }
        SQLiteModule::vSQLBackBuffer.push_back(strResult);
      }
      SQLiteModule::evEvent.Set();
      return 0;
    }
    
    static void* RunServer(void* arg);
    static void* ClientConnection(void* arg);

  public:

    ~SQLiteModule();  
  
    static SQLiteModule& GetInstance()
    {
      static SQLiteModule instance;
      return instance;
    }

    static int StartServer()
    {
      return pthread_create(&SQLiteModule::tServerId, NULL, SQLiteModule::RunServer, NULL); 
    }    
    
    int Init(const std::string& strdbFileName);
    void Close();
    const std::vector<std::string> Query(const std::string& strQuery) throw(SQLiteException);
};

#endif // _SQLITEMODULE_H_
