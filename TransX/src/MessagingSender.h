#ifndef MESSAGINGSENDER_H
#define MESSAGINGSENDER_H

#include <EnjoLib/ModuleMessagingExtPut.hpp>

class MessagingSender : public EnjoLib::ModuleMessagingExtPut
{
    public:
        MessagingSender();
        virtual ~MessagingSender();
        const char * ModuleMessagingGetModuleName() const;
    protected:
    private:
};

#endif // MESSAGINGSENDER_H
