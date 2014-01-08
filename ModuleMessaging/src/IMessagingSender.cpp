#include "IMessagingSender.hpp"
#include "Storage.hpp"

using namespace EnjoLib;

IMessagingSender::IMessagingSender()
{}

IMessagingSender::~IMessagingSender()
{}

void IMessagingSender::Send(const char * varName, double var) const
{
    Storage::Store(*this, varName, var);
}

void IMessagingSender::Send(const char * varName, const VECTOR3 & var) const
{
    Storage::Store(*this, varName, var);
}
