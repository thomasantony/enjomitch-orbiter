/***************************************************************
 * Name:      da_solverApp.h
 * Purpose:   Defines Application Class
 * Author:    Szymon Ender ()
 * Created:   2009-12-20
 * Copyright: Szymon Ender ()
 * License:
 **************************************************************/

#ifndef DA_SOLVERAPP_H
#define DA_SOLVERAPP_H

#ifndef WX_PRECOMP
  #include <wx/app.h>
#endif

class da_solverApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // DA_SOLVERAPP_H
