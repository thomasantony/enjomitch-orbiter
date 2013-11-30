#ifndef MYMFDBUTTONSPAGE_H
#define MYMFDBUTTONSPAGE_H

#include "../../MFDButtonPage.hpp"
#include "MyMFDMultipleVessels.hpp"

class MyMFDButtonsPage : public EnjoLib::MFDGoodies::MFDButtonPage<MyMFDMultipleVessels>
{
    public:
        MyMFDButtonsPage();
        virtual ~MyMFDButtonsPage();
    protected:
        bool SearchForKeysInOtherPages() const;
    private:
};

#endif // MYMFDBUTTONSPAGE_H
