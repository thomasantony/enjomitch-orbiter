#ifndef GREATCIRCLEANIMATED_H
#define GREATCIRCLEANIMATED_H

#include "GreatCircle.h"
#include "PID/PID.hpp"


class GreatCircleAnimated : public GreatCircle
{
    public:
        GreatCircleAnimated();
        virtual ~GreatCircleAnimated();

        void ZoomIn();
        void ZoomOut();
        void Update( MyDC hDC, int W, int H, MFDDataLaunchMFD * data );

    protected:
        void AutoZoomVirtual();

    private:
        void UpdateZoom();

        double m_tgtZoom;
        static double m_prevTime;
        EnjoLib::PID m_pidZoomVel;
};

#endif // GREATCIRCLEANIMATED_H
