#include <iostream>

#include <wx/string.h>
#include <wx/socket.h>

int main()
{
  wxString strAddr((wxChar*)"localhost");
  wxIPV4address addrServer;
  wxSocketClient sockClient;
  addrServer.Hostname(strAddr);
  addrServer.Service(1234);
  sockClient.Connect(addrServer);
  if (sockClient.IsConnected()) {
    std::cout << "Client connected" << std::endl;
  }
  sockClient.Close();
  std::cout << "Press any key to exit" << std::endl;  
  std::cin.get();
  return 0;
}
