//#include "ptree.h"
#ifdef _WIN32
  #include <windows.h>
  static CRITICAL_SECTION cs_sqlite;
#endif

#include "sqlitemodule1.h"

sqlite3 *sql_db=NULL;
char *zErrMsg;
vector<string > sqlbackbuffer;



int initialise_sqlite(string dbfile)
{
	int result=0;
	EnterCriticalSection(&cs_sqlite);
	result=sqlite3_open (dbfile.c_str (), &sql_db);
	LeaveCriticalSection(&cs_sqlite);
	return result;
}

void close_sqlite()
{
	if(NULL!=sql_db)
	{
		EnterCriticalSection(&cs_sqlite);
		sqlite3_close (sql_db);
		LeaveCriticalSection(&cs_sqlite);
	}
}

static int callback (void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	string tmp;
	for (i = 0; i < argc; i++)
	{
		if (i < (argc - 1))
			tmp = string(argv[i] ? argv[i] : "NULL");
		else
			//tmp = string(argv[i] ? argv[i] : "\r\n");
      tmp = string(argv[i] ? argv[i] : "");  // we don't need a newline
		sqlbackbuffer.push_back(tmp);
	}
	return 0;
}

bool sql_run_file(string sqlfile)
{
/*
  if (FileExists (sqlfile))
  {
    TStringList *buffer = new TStringList ();
    buffer->LoadFromFile (sqlfile);
    for (int i = 0; i < buffer->Count; i++)
      do_sql_query(buffer->Strings[i]);

    delete buffer;
    return true;
  }
  else */
    return false;

}


int do_sql_query(const string & q)
{
	int rc=SQLITE_ERROR;
	if(NULL!=sql_db)
	{
		EnterCriticalSection(&cs_sqlite);
		sqlbackbuffer.clear();
		rc = sqlite3_exec (sql_db, q.c_str (), callback, 0, &zErrMsg);
		LeaveCriticalSection(&cs_sqlite);
	}
	return rc;
}


/**
* @author Dave Rowbotham.
*
* @param *Sender TObject
*/


string do_sql_colquery(const string & q, vector<string> *sql_colresult)
{
	string sret;
	int rc=SQLITE_ERROR;
	if(NULL!=sql_db && NULL!=sql_colresult)
	{
		EnterCriticalSection(&cs_sqlite);
		sqlbackbuffer.clear();
		rc = sqlite3_exec (sql_db, q.c_str (), callback, 0, &zErrMsg);
		if (rc == SQLITE_OK)
		{
			for (size_t i = 0; i < sqlbackbuffer.size(); i++)
				sql_colresult->push_back (sqlbackbuffer[i]);
			sqlbackbuffer.clear();
		}
		else
		{
			sret = zErrMsg;
			sqlbackbuffer.clear();
		}
		LeaveCriticalSection(&cs_sqlite);
	}
  return sret;
}

/// some flaws in the below function
vector<string> do_sql_colquery(const string & q)
{
	int rc=SQLITE_ERROR;
	vector<string> sql_colresult;
	if(sql_db!=NULL)
	{
		EnterCriticalSection(&cs_sqlite);
		sqlbackbuffer.clear();
		rc = sqlite3_exec (sql_db, q.c_str (), callback, 0, &zErrMsg);
		if (rc != SQLITE_OK)
			return sql_colresult;
		else
		{
			for (size_t i = 0; i < sqlbackbuffer.size(); i++)
				sql_colresult.push_back (sqlbackbuffer[i]);

			sqlbackbuffer.clear();
		}
		LeaveCriticalSection(&cs_sqlite);
	}
	return sql_colresult;

}

/* for parsed data, e.g., the name of datasets or groups */
char *xml_escape_name_string(const char * str)
{
  int extra;
  int len;
  int i;
  char * cp;
  char * ncp;
  char * rcp;

  if (str == NULL)
    return (char *)str;
  cp = (char *)str;
  len = strlen(str);
  extra = 0;
  for ( i = 0;i < len; i++ )
  {
      if (*cp == '\"')
      {
        extra += (strlen("\"") - 1);
      }
      else if (*cp == '\'')
      {
        extra += (strlen("&apos;") - 1);
      }
      else if (*cp == '<')
      {
        extra += (strlen("&lt;") - 1);
      }
      else if (*cp == '>')
      {
        extra += (strlen("&gt;") - 1);
      }
      else if (*cp == '&')
      {
        extra += (strlen("&amp;") - 1);
      }
    cp++;
  }

  if (extra == 0)
  {
    return (char *)str;
  }
  else
  {
    cp = (char *)str;
    rcp = ncp =(char*) malloc(len+extra+1);
    if (ncp == NULL)
      return NULL; /* ?? */
    for (i = 0; i < len; i++)
    {
      if (*cp == '\'')
      {
        strncpy(ncp,"&apos;",strlen("&apos;"));
        ncp += strlen("&apos;");
        cp++;
      }
      else if (*cp == '<')
      {
        strncpy(ncp,"&lt;",strlen("&lt;"));
        ncp += strlen("&lt;");
        cp++;
      }
      else if (*cp == '>')
      {
        strncpy(ncp,"&gt;",strlen("&gt;"));
        ncp += strlen("&gt;");
        cp++;
      }
      else if (*cp == '\"')
      {
        strncpy(ncp,"\"",strlen("\""));
        ncp += strlen("\"");
        cp++;
      }
      else if (*cp == '&')
      {
        strncpy(ncp,"&amp;",strlen("&amp;"));
        ncp += strlen("&amp;");
        cp++;
      }
      else
      {
        *ncp++ = *cp++;
      }
    }
    *ncp = '\0';
    return rcp;
  }
}

/* for unparsed data, e.g., String data */
char *xml_escape_string(const char * str)
{
  int extra;
  int len;
  int i;
  char * cp;
  char * ncp;
  char * rcp;

  if (str == NULL)
    return (char *)str;
  cp = (char *)str;
  len = strlen(str);
  extra = 0;
  for ( i = 0;i < len; i++ )
  {
    if (*cp == '\\')
    {
      extra++;
    }
    else if (*cp == '\"')
    {
    extra ++;
    }
    else if (*cp == '\'')
    {
    extra += (strlen("&apos;") - 1);
    }
    else if (*cp == '<')
    {
    extra += (strlen("&lt;") - 1);
    }
    else if (*cp == '>')
    {
    extra += (strlen("&gt;") - 1);
    }
    else if (*cp == '&')
    {
      extra += (strlen("&amp;") - 1);
    }
    cp++;
  }

  if (extra == 0)
  {
    return (char *)str;
  }
  else
  {
    cp = (char *)str;
    rcp = ncp = (char*)malloc(len+extra+1);
    if (ncp == NULL)
      return NULL; /* ?? */
    for (i = 0; i < len; i++)
    {
      if (*cp == '\\')
      {
        *ncp++ = '\\';
        *ncp++ = *cp++;
      }
      else if (*cp == '\"')
      {
        *ncp++ = '\\';
        *ncp++ = *cp++;
      }
      else if (*cp == '\'')
      {
        strncpy(ncp,"&apos;",strlen("&apos;"));
        ncp += strlen("&apos;");
        cp++;
      }
      else if (*cp == '<')
      {
        strncpy(ncp,"&lt;",strlen("&lt;"));
        ncp += strlen("&lt;");
        cp++;
      }
      else if (*cp == '>')
      {
        strncpy(ncp,"&gt;",strlen("&gt;"));
        ncp += strlen("&gt;");
        cp++;
      }
      else if (*cp == '&')
      {
        strncpy(ncp,"&amp;",strlen("&amp;"));
        ncp += strlen("&amp;");
        cp++;
      }
      else
      {
        *ncp++ = *cp++;
      }
    }
  *ncp = '\0';
  return rcp;
  }
}



