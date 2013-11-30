#include <iostream>
#include "../common/serverthread.h"

int main()
{
  Thread* pThread = Thread::Create();
  pThread->Run();
  std::cout << "Thread is " << (pThread->GetState() == Thread::RUNNING ? "running" : "stopped") << std::endl;
  std::cout << "Press any key to exit" << std::endl; 
  std::cin.get();
  Thread::Destroy(pThread); 
  return 0;
}
