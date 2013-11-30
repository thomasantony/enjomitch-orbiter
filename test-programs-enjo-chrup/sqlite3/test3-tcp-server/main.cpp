#include "../common/sqlitemodule.h"
#include "../common/sqlite3.h"

#include <iostream>

int main ()
{    
  SQLiteModule::GetInstance().Init("dbtest.db");
  SQLiteModule::GetInstance().StartServer();
  std::cout << "Hit any key!" << std::endl;
  std::cin.get();
  return 0;
}
