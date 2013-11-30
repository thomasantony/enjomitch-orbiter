#define STRICT

#include "OptimiserVar.h"
#include <dlib/optimization.h>
#include "basefunction.h"


OptimiserVar::OptimiserVar()
{
    //ctor
}

OptimiserVar::~OptimiserVar()
{
    //dtor
}

void OptimiserVar::Init( basefunction * baseFunc )
{
    m_baseFunc = baseFunc;
}
