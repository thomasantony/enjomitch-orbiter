#ifndef MYDC_H
#define MYDC_H

#include <string>
#include <orbitersdk.h>

#ifdef ORB2006
    typedef HDC MyDC;
    typedef HPEN MyPEN;
#else
    #ifdef ORB2009
        typedef oapi::Sketchpad * MyDC;
        typedef oapi::Pen * MyPEN;
    #endif
#endif


#define GREEN		RGB(0x00, 0xFF, 0x00)
#define YELLOW		RGB(0xDF, 0xDF, 0x00)
#define	WHITE		RGB(0xFF, 0xFF, 0xFF)
#define RED			RGB(0xFF, 0x00, 0x00)
#define GREY		RGB(0xE0, 0xE0, 0xE0)
#define LIGHT_BLUE	RGB(0x22, 0xDD, 0xFF)


// Macro for doing a more maneagable TextOut.
// Use as TextOut, except specifying the colour as you would with SetTextColour. If Colour is 0, it will use the old colour
// The ... (__VA_ARGS__) can be used as the extra formatting parameters as you would use for sprintf.
// No need to specify text length - it will get it from strlen.
// eg.: MFDTextOut(hDC, 30, 20, GREEN, "Pi is equal to %lf", M_PI);
// Author: Steve "agentgonzo" Arch
#ifdef ORB2006
#define MFDTextOut(hDC, x, y, colour, text, ...)	{char buf[1024]; sprintf_s(buf, 1024, text, __VA_ARGS__); \
														if(colour != 0)\
															SetTextColor(hDC, colour);\
														TextOut(hDC, x, y, buf, (int)strlen(buf));\
													}
#else
#define MFDTextOut(skp, x, y, colour, text, ...)	{char buf[1024]; sprintf_s(buf, 1024, text, __VA_ARGS__); \
														if(colour != 0)\
															skp->SetTextColor(colour);\
														skp->Text(x, y, buf, (int)strlen(buf));\
													}
#endif


#ifdef ORB2006
#define MFDLine(hDC, x0, y0, x1, y1)	{			MoveToEx(hDC, (int)(x0), (int)(y0), NULL);\
														LineTo(hDC, (int)(x1),	(int)(y1)	);\
													}
#else
#define MFDLine(skp, x0, y0, x1, y1)	{	skp->Line(x0, y0, x1, y1);\
										}

#endif
#ifdef ORB2006
#define MFDPoint(hDC, x, y)	{			MoveToEx(hDC, (int)(x), (int)(y), NULL);\
														LineTo(hDC, (int)(x+1),	(int)(y)	);\
													}
#else
#define MFDPoint(skp, x, y)	{	skp->Line(x, y, x+1, y);\
										}
#endif

#ifdef ORB2006
    #define MFDSetPen(hDC, p) { SelectObject(hDC, p); }
#else
    #define MFDSetPen(skp, p) { skp->SetPen(p); }
#endif

#ifdef ORB2006
    #define MFDTextAlignCenter(hDC) { SetTextAlign(hDC, TA_CENTER); }
#else
    #define MFDTextAlignCenter(skp) { skp->SetTextAlign( oapi::Sketchpad::CENTER, oapi::Sketchpad::BASELINE ); }
#endif


#endif // MYDC_H
