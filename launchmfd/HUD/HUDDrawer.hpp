#ifndef HUDDRAWER_H
#define HUDDRAWER_H

#include <EnjoLib/IDrawsHUD.hpp>
#include <Utils/MyDC.h>
#include "HUDCompass.h"

class MFDDataLaunchMFD;
class HUDDrawer : public EnjoLib::IDrawsHUD
{
    public:
        HUDDrawer();
        virtual ~HUDDrawer();

        void SetMFDData( MFDDataLaunchMFD * data );
        bool ShouldDrawHUD() const;
        void DrawHUD(int mode, const HUDPAINTSPEC *hps, MyDC hDC);
        const char * GetModuleName() const;

    protected:
    private:

        MFDDataLaunchMFD * m_data;
        HUDCompass HUD;
};

#endif // HUDDRAWER_H
