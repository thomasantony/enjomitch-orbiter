#ifndef _SQLITEMODULE1_H_
#define _SQLITEMODULE1_H_

#include <string>
#include <vector>

#include "sqlite3.h"

#define EnterCriticalSection(x) ; // TODO!!!

using namespace std;

// Initlialise sqlite with a database filename (any extension)
int initialise_sqlite(string dbfile);

// Call when closing program or before changing a database
void close_sqlite();

// execute an SQL query
string do_sql_colquery(const string & q, vector<string> *sql_colresult);

char *xml_escape_name_string(const char * str);
char *xml_escape_string(const char * str);

//bool sql_run_file(string sqlfile); // unimplemented
#endif // _SQLITEMODULE1_H_
