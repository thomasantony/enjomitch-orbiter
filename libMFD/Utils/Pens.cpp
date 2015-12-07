#include "Pens.h"
#include <stdexcept>

Pens::Pens()
{
#ifdef ORB2006
    AddPen( Green,          CreatePen(PS_SOLID, 0, GREEN) );
    AddPen( Yellow,         CreatePen(PS_SOLID, 0, YELLOW) );
    AddPen( GreenDashed,    CreatePen(PS_DASH,  0, GREEN) );
    AddPen( White,          CreatePen(PS_SOLID, 0, WHITE) );
    AddPen( Grey,           CreatePen(PS_SOLID, 0, GREY) );
    AddPen( Red,            CreatePen(PS_SOLID, 0, RED) );
#else
    // Allocate pens themselves
    AddPen( Green,          oapiCreatePen(1, 1, GREEN) );
    AddPen( Yellow,         oapiCreatePen(1, 1, YELLOW) );
    AddPen( GreenDashed,    oapiCreatePen(2, 1, GREEN) );
    AddPen( White,          oapiCreatePen(1, 1, WHITE) );
    AddPen( Grey,           oapiCreatePen(1, 1, GREY) );
    AddPen( Red,            oapiCreatePen(1, 1, RED) );
#endif
}

// Destructor
// Guaranteed to be executed when class leaves scope
Pens::~Pens()
{
    // Deallocate pens
    for(int i = 0; i < cNumPens; ++i)
    {
#ifdef ORB2006
        DeleteObject(m_pens[i]);
#else
        oapiReleasePen(m_pens[i]);
#endif
    }
}

const MyPEN & Pens::GetPen(LineStyle style) const
{
    if ( style >= cNumPens )
    {
        // Whoops! You forgot to increment cNumPens after adding a new LineStyle.
        // Must halt the application.
        throw std::invalid_argument("Pens::GetPen(): out of bounds");
    }
    return m_pens[style];
}

void Pens::AddPen( LineStyle style, MyPEN pen )
{
    if ( style >= cNumPens )
        throw std::invalid_argument("Pens::AddPen(): out of bounds");
    m_pens[style] = pen;
}
