#ifndef PENS_H
#define PENS_H

#include "MyDC.h"

class Pens
{
    public:
        Pens();
        virtual ~Pens();

        // Use this enum select the pen that you want with a call like:
        // Pens::GetPen( Pens::Green );
        enum LineStyle
        {
            Green,
            Yellow,
            GreenDashed,
            White,
            Grey,
            Red
        };

        // Class' interface - returns a constant reference to MyPEN
        // You can select the pen you want from the LineStyle
        const MyPEN & GetPen(LineStyle style) const;
    protected:
    private:
        // Declaring copy constructor and assignment operator private makes the class non-copyable
        // This takes away the pain of manual memory management. If you need the class again,
        // just declare it again.
        Pens(const Pens & other);
        Pens & operator=(const Pens & other);

        // Adds a pen with bounds checking
        void AddPen( LineStyle style, MyPEN pen );

        const static int cNumPens = 6; // keep this number in order by counting the number of LineStyle colors
        MyPEN m_pens[cNumPens]; // an array of pens.
};

#endif // PENS_H
