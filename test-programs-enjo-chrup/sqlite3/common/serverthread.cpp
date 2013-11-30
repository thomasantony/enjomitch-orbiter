#include <wx/wx.h>

#include "serverthread.h"

#define _DEBUG
#ifdef _DEBUG
  #include <iostream>
#endif

void ServerThread::OnRun()
{
  wxIPV4address addrServer;
  if (addrServer.AnyAddress()) {
    if (addrServer.Service(1234)) {
      wxSocketServer sockServer(addrServer);
      if (sockServer.IsOk()) {
        std::cout << "Listening: " << std::endl; 
        while (true) {
          std::vector<ClientThread*>::iterator iterClient;
          ClientThread* pClientThread;
          wxSocketBase* sockClient = sockServer.Accept();
          // remove all non communicating clients
          for (iterClient = this->m_vClient.begin(); iterClient != this->m_vClient.end(); ++iterClient) {
            ClientThread* pClientThread = *iterClient;
            if (pClientThread->GetState() == Thread::STOPPED) {
              this->m_vClient.erase(iterClient);
              ClientThread::Destroy(pClientThread);
            }
          }
          if (this->m_vClient.size() < this->nMaxClients) {
            pClientThread = ClientThread::Create(sockClient);
            this->m_vClient.push_back(pClientThread);
            pClientThread->Run();
          }
        }
      } else {
        throw ThreadException("ServerThread::OnRun() -> wxSocketServer::IsOk()");
      }
    } else {
      throw ThreadException("ServerThread::OnRun() -> wxIPV4address::Service()");
    }
  } else {  
    throw ThreadException("ServerThread::OnRun() -> wxIPV4address::AnyAddress()");
  }  
}

void ServerThread::ClientThread::OnRun()
{ 
  wxIPV4address addrClient;
  if (!this->m_sockClient) {
    ThreadException("ServerThread::OnRun() -> wxIPV4address::Service()");
  }
  if (this->m_sockClient->GetLocal(addrClient)) {
    std::cout << addrClient.IPAddress().mb_str() << " connected!" << std::endl;
  }
  this->DestroySocket();
  std::cout << "Client stopped!" << std::endl;
}

ServerThread::~ServerThread() throw() 
{
  std::vector<ClientThread*>::iterator iterClient;
  for (iterClient = this->m_vClient.begin(); iterClient != this->m_vClient.end(); ++iterClient) {
    ClientThread* pClientThread = *iterClient;
    ClientThread::Destroy(pClientThread);
  }
  this->m_vClient.clear();
}

Thread* ServerThread::Create() 
{
  Thread* pThread = new ServerThread;

#ifdef _DEBUG
  std::cout << "Thread created: " << pThread << std::endl;
#endif

  return pThread;
}

ServerThread::ClientThread* ServerThread::ClientThread::Create(wxSocketBase* sockClient) 
{ 
  ServerThread::ClientThread* pThread = new ClientThread;
  pThread->m_sockClient = sockClient;
  return pThread; 
}

void ServerThread::ClientThread::Destroy(ServerThread::ClientThread* pThread) 
{
#ifdef _DEBUG 
  std::cout << "Client Thread deleted: " << pThread << std::endl;
#endif

  delete pThread; 
}

ServerThread::ClientThread::~ClientThread() throw() 
{ 
  this->Stop();
  this->m_muxSocket.Unlock();  
  this->DestroySocket();
}

void ServerThread::ClientThread::DestroySocket()
{
  this->m_muxSocket.Lock();
  if (this->m_sockClient) {
#ifdef _DEBUG   
    std::cout << "Socket destroyed" << std::endl;
#endif

    this->m_sockClient->Destroy();
    this->m_sockClient = NULL;
  } 
  this->m_muxSocket.Unlock();
}
