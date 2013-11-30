#include <windows.h>

#include <iostream>
#include <string>

#include "../common/sqlitemodule.h"

void InsertUser(const std::string& newUser)
{
  std::vector<std::string> vResult;
  bool found = false;
  vResult = SQLiteModule::GetInstance().Query("select username from users;");  
  for (std::vector<std::string>::size_type i = 0; i < vResult.size(); ++i) {
    if (vResult.at(i) == newUser) {
      found = true;
      break;
    }
  }
  if (!found) {
    std::string strQuery = "insert into users values (NULL, \"";
    strQuery += newUser;
    strQuery += "\");";
    std::cout << "query = " << strQuery << std::endl;
    vResult = SQLiteModule::GetInstance().Query(strQuery);
  }
}

int main ()
{ 
  std::vector<std::string> vResult;
  SQLiteModule::GetInstance().Init("dbtest.db");
  std::cout << "Passed Init" << std::endl;  
  SQLiteModule::GetInstance().Query("create table users(userid integer PRIMARY KEY, username text);");
  std::cout << "Passed Create Table" << std::endl;  
  InsertUser("enjo");
  InsertUser("chrupek");
  vResult = SQLiteModule::GetInstance().Query("select * from users;");
  std::cout << "size = " << vResult.size() << std::endl;
  for (std::vector<std::string>::size_type i = 0; i < vResult.size(); ++i) {
    std::cout << "Result "<< i <<" = " << vResult.at(i) << std::endl; 
  }
  /*
  try {  
    SQLiteModule::GetInstance().Query("selecfrom users;");
  } catch (SQLiteException& ex) {
    std::cout << ex.what() << std::endl;
  }
  */
  return 0;
}
