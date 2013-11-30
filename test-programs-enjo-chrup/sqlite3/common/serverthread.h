#ifndef __SERVERTHREAD_H_
#define __SERVERTHREAD_H_

#include <wx/socket.h>
#include <vector>

#include "thread.h"

class ServerThread : public Thread
{
    class ClientThread : public Thread
    {
        ClientThread() : m_sockClient(NULL) {}
        virtual ~ClientThread() throw();

        wxSocketBase* m_sockClient;
        Mutex         m_muxSocket;

      protected:
        virtual void OnRun();

        void DestroySocket();

      public:
        static ClientThread* Create(wxSocketBase* sockClient);
        static void Destroy(ClientThread* pThread);

    };    

    ServerThread() {}
    virtual ~ServerThread() throw();
    
    std::vector<ClientThread*> m_vClient;
    static const unsigned nMaxClients = 1;

  protected:

    virtual void OnRun();

  public:
      static Thread* Create();
};

#endif // __SERVERTHREAD_H_
