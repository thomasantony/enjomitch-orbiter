#ifndef PTRHELPER_H
#define PTRHELPER_H


class PtrHelper
{
    public:
        PtrHelper();
        virtual ~PtrHelper();

        static void ThrowIfCondMet(bool condition, const char * typeName, const char * subTypeName);

    protected:

    private:
};

#endif // PTRHELPER_H
