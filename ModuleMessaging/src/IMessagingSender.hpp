#ifndef IMESSAGINGSENDER_H
#define IMESSAGINGSENDER_H

#include <OrbiterSdk.h>

namespace EnjoLib
{
class __declspec(dllexport) IMessagingSender
{
    public:
        IMessagingSender();
        virtual ~IMessagingSender();
        virtual const char * GetModuleName() const = 0;

        void Send(const char * varName, bool var) const;
        void Send(const char * varName, int var) const;
        void Send(const char * varName, double var) const;
        void Send(const char * varName, const VECTOR3 & var) const;
        void Send(const char * varName, const MATRIX3 & var) const;
        void Send(const char * varName, const MATRIX4 & var) const;
        void Send(const char * varName, const char * var) const;

    protected:

    private:
};
}

#endif // IMESSAGINGSENDER_H
