#ifndef MESSAGINGSENDER_H
#define MESSAGINGSENDER_H

#include <EnjoLib/IMessagingSender.hpp>

class MessagingSender : public EnjoLib::IMessagingSender
{
    public:
        MessagingSender();
        virtual ~MessagingSender();
        const char * GetModuleName() const;
    protected:
    private:
};

#endif // MESSAGINGSENDER_H
