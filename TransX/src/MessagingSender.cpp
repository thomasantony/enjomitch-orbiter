#include "MessagingSender.h"

const MMExt2::Basic MessagingSender::m_mmext("TransX");

MessagingSender::MessagingSender()
{
}

MessagingSender::~MessagingSender()
{
}

void MessagingSender::ModMsgPut(const char * varName, double val) const
{
    m_mmext.Put(varName, val);
}

void MessagingSender::ModMsgPut(const char * varName, const VECTOR3 & val) const
{
    m_mmext.Put(varName, val);
}

/*
const char * MessagingSender::ModuleMessagingGetModuleName() const
{
    return "TransX";
}
*/
