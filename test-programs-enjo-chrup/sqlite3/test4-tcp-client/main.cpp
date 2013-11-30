#include <iostream>
#include <winsock2.h>

void Send(const std::string& strServer, int nPortNo, const std::string& strQuery) 
{
  SOCKET Socket;
  struct sockaddr_in ServerAddress;
  struct hostent *Server;  
  //Create a socket
  Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (Socket != INVALID_SOCKET) {
    //Server name will be supplied as a commandline argument
    //Get the server details
    Server = gethostbyname(strServer.c_str());
    if (Server != NULL) {
      //Cleanup and Init with 0 the ServerAddress
      ZeroMemory((char *) &ServerAddress, sizeof(ServerAddress));
      ServerAddress.sin_family = AF_INET;
      //Assign the information received from gethostbyname()
      CopyMemory((char *)&ServerAddress.sin_addr.s_addr, (char *)Server->h_addr, Server->h_length);
      ServerAddress.sin_port = htons(nPortNo); //comes from commandline
      //Establish connection with the server          
      if (connect(Socket, reinterpret_cast<const struct sockaddr *>(&ServerAddress), sizeof(ServerAddress)) != SOCKET_ERROR) {
        int nBytesSent;
        int nBytesRecv;          
        //Send the message to the server
        nBytesSent = send(Socket, strQuery.c_str(), strQuery.length(), 0);
/*
        if (nBytesSent != SOCKET_ERROR) {
          //Cleanup and Init with 0 the szBuffer
          ZeroMemory(szBuffer,256);
          //Get the message from the server
          nBytesRecv = recv(Socket, szBuffer, 255, 0 );
          if (nBytesRecv != SOCKET_ERROR) {
            //Display the server message
            std::cout << szBuffer << std::endl;
            //Close the socket
          }
        }
 */      
      }
    }
  }
  closesocket(Socket);
}

int main ()
{    
  std::string strServer = "127.0.0.1";
  int nPortNo = 1234;
  std::string strQuery = "select * from users;";
  // Initialize Winsock
  WSADATA wsaData;
  int nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (nResult == NO_ERROR) {
    Send(strServer, nPortNo, strQuery); 
    //Cleanup Winsock
    WSACleanup();
  }
  return 0;
}
