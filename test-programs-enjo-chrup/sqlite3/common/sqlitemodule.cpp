#include "sqlitemodule.h"

std::vector<std::string>                SQLiteModule::vSQLBackBuffer;
pthread_t                               SQLiteModule::tServerId;
bool                                    SQLiteModule::bServerIsRunning = false;
SQLiteModule::Mutex                     SQLiteModule::mMutex;

SQLiteModule::Mutex                     SQLiteModule::mClientConnectionsMutex;
std::queue<SQLiteModule::ClientParam *> SQLiteModule::qClientConnections;
unsigned                                SQLiteModule::nConnections = 0;

SQLiteModule::CEvent                    SQLiteModule::evEvent(true, false);

sqlite3*                                SQLiteModule::dbSQL = NULL;
char*                                   SQLiteModule::zErrMsg = NULL;    
    
SQLiteModule::SQLiteModule() {}

SQLiteModule::~SQLiteModule() 
{
  if (SQLiteModule::dbSQL) {
    this->Close();
  }
}

int SQLiteModule::Init(const std::string& strdbFileName)
{
	int nResult = 0;
  SQLiteModule::mMutex.Lock();
	nResult = sqlite3_open(strdbFileName.c_str (), &SQLiteModule::dbSQL);
  SQLiteModule::mMutex.Unlock();
	return nResult;
}

void SQLiteModule::Close()
{
	if (SQLiteModule::dbSQL) {
    SQLiteModule::mMutex.Lock();
		sqlite3_close(SQLiteModule::dbSQL);
    SQLiteModule::mMutex.Unlock();
    SQLiteModule::dbSQL = NULL;
	}
}

const std::vector<std::string> SQLiteModule::Query(const std::string& strQuery) throw(SQLiteException)
{
	int nRC = SQLITE_ERROR;
	if (SQLiteModule::dbSQL) {
    SQLiteModule::evEvent.Reset();
		SQLiteModule::mMutex.Lock();
    SQLiteModule::vSQLBackBuffer.clear();
    nRC = sqlite3_exec(SQLiteModule::dbSQL, strQuery.c_str(), SQLiteModule::SQLiteCallback, 0, &SQLiteModule::zErrMsg);
    if (nRC != SQLITE_OK) {
      //throw(SQLiteException(this->m_zErrMsg));
    }
    SQLiteModule::mMutex.Unlock();
    SQLiteModule::evEvent.Wait();
	}
  return SQLiteModule::vSQLBackBuffer;
}  

void* SQLiteModule::RunServer(void* arg)
{
  SQLiteModule::bServerIsRunning = true;
  // Initialize Winsock
  WSADATA wsaData;
  int nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (nResult == NO_ERROR) {
    SOCKET ListenSocket, RemoteSocket; 
    int nPortNo, nClientLength;
    char szBuffer[256];
    struct sockaddr_in ServerAddress, ClientAddress; 
    //Create a socket
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket != INVALID_SOCKET) {    
      //Cleanup and Init with 0 the ServerAddress
      ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
      //Port number will be supplied as a commandline argument
      nPortNo = atoi("1234");     
      //Fill up the address structure
      ServerAddress.sin_family = AF_INET;
      ServerAddress.sin_addr.s_addr = INADDR_ANY; //WinSock will supply address
      ServerAddress.sin_port = htons(nPortNo);    //comes from commandline     
      //Assign local address and port number
      if (bind(ListenSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)) != SOCKET_ERROR) {
        while (true) {
          //Make the socket a listening socket
          if (listen(ListenSocket, SOMAXCONN) != SOCKET_ERROR) {
            nClientLength = sizeof(ClientAddress);
            //Accept remote connection attempt from the client
            RemoteSocket = accept(ListenSocket, (struct sockaddr *) &ClientAddress, &nClientLength);
            if (RemoteSocket != INVALID_SOCKET) {
              bool bCanAccept = false;
              SQLiteModule::mClientConnectionsMutex.Lock();
              if (SQLiteModule::nConnections < SQLiteModule::nMaxConnections) {
                ++SQLiteModule::nConnections;
                bCanAccept = true;
              }
              SQLiteModule::mClientConnectionsMutex.Unlock();
              if (bCanAccept) {
                pthread_t tClientId;
                SQLiteModule::ClientParam* pClient;
                //Display Client's IP
                std::cout << "Client connected from: " << inet_ntoa(ClientAddress.sin_addr) << std::endl;
                pClient = new SQLiteModule::ClientParam;
                pClient->ListenSocket = ListenSocket;
                pClient->RemoteSocket = RemoteSocket;
                SQLiteModule::mClientConnectionsMutex.Lock();
                SQLiteModule::qClientConnections.push(pClient);
                SQLiteModule::mClientConnectionsMutex.Unlock();
                pthread_create(&tClientId, NULL, SQLiteModule::ClientConnection, NULL);
              } else {
                //Display Client's IP
                std::cout << "Client connection refused from: " << inet_ntoa(ClientAddress.sin_addr) << std::endl;
              }
            } else {
              std::cout << "\tError occurred while accepting socket: " << WSAGetLastError() << std::endl;           
            }
          } else {
            std::cout << "\tError occurred while listen socket: " << WSAGetLastError() << std::endl;           
          }   
        }
      }
      closesocket(ListenSocket);
    } else {
      std::cout << "\tError occurred while opening socket: " << WSAGetLastError() << std::endl;
    } 
    // Cleanup Winsock
    WSACleanup();    
  }
  SQLiteModule::bServerIsRunning = false;
  std::cout << "Server stopped" << std::endl;
  return NULL;
}  

void* SQLiteModule::ClientConnection(void* pArg)
{
  std::vector<char> vQuery;
  char zBuffer;              
  int nBytesSent, nBytesRecv;
  SOCKET ListenSocket, RemoteSocket;   
  bool bEndOfTransmission = false;

  SQLiteModule::ClientParam* pClient;
  SQLiteModule::mClientConnectionsMutex.Lock();
  pClient = SQLiteModule::qClientConnections.back();
  SQLiteModule::qClientConnections.pop();
  ListenSocket = pClient->ListenSocket; 
  RemoteSocket = pClient->RemoteSocket; 
  delete pClient;
  SQLiteModule::mClientConnectionsMutex.Unlock();

  //Receive data from a connected or bound socket
  while (!bEndOfTransmission) {
    nBytesRecv = recv(RemoteSocket, &zBuffer, 1, 0 );
    if (nBytesRecv != SOCKET_ERROR) {
      if (zBuffer != ';') {
        vQuery.push_back(zBuffer);
      } else {                   
        bEndOfTransmission = true;
      }
    } else {
      bEndOfTransmission = true;
    }
  }
  for (std::vector<char>::size_type i=0; i<vQuery.size(); ++i) {
    std::cout << vQuery.at(i);
  }
  std::cout << std::endl;  
  closesocket(RemoteSocket);  
  SQLiteModule::mClientConnectionsMutex.Lock();  
  --SQLiteModule::nConnections;  
  SQLiteModule::mClientConnectionsMutex.Unlock();   
  //nBytesSent = send(RemoteSocket, "dummy", strlen("dummy"), 0);
  return NULL;
}
