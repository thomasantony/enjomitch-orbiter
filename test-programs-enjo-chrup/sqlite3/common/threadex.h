#ifndef __THREADEX_H_
#define __THREADEX_H_

#include <exception>
#include <string>

class ThreadException : public std::exception
{
    std::string m_strWhat;

  public:
  
    ThreadException(const std::string& strWhat) : m_strWhat(strWhat) {}
    virtual ~ThreadException() throw() {}
    
    virtual const char* what() const throw() { return m_strWhat.c_str(); }
};

#endif // __THREADEX_H_
