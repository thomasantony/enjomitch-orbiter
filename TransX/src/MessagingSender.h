#ifndef MESSAGINGSENDER_H
#define MESSAGINGSENDER_H

#include <MMExt2_Basic.hpp>

class MessagingSender
{
    public:
        MessagingSender();
        virtual ~MessagingSender();

        void ModMsgPut(const char * varName, double val) const;
        void ModMsgPut(const char * varName, const VECTOR3 & val) const;
        //const char * ModuleMessagingGetModuleName() const;
    protected:
    private:

        static const MMExt2::Basic m_mmext;
};

#endif // MESSAGINGSENDER_H
