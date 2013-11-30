#include "GreatCircleAnimated.h"

// Has to be static, so that in multiple vessels it doesn't act silly
double GreatCircleAnimated::m_prevTime = oapiGetSysTime();

GreatCircleAnimated::GreatCircleAnimated()
: m_pidZoomVel(1.5)
{
}

GreatCircleAnimated::~GreatCircleAnimated()
{
}

void GreatCircleAnimated::ZoomIn()
{
    if ( m_track )
        m_tgtZoom *= m_zoomMultipler;
}

void GreatCircleAnimated::ZoomOut()
{
    if ( m_track )
    {
        double newValue = m_tgtZoom / m_zoomMultipler;
        m_tgtZoom = GetMinAllowedZoom( newValue );
    }
}

void GreatCircleAnimated::Update( MyDC hDC, int W, int H, MFDDataLaunchMFD * data )
{
    GreatCircle::Update( hDC, W, H, data );
    UpdateZoom();
}

void GreatCircleAnimated::AutoZoomVirtual()
{
    Canvas::AutoZoom();
    m_tgtZoom = CalculateAutoZoomValue();
}

void GreatCircleAnimated::UpdateZoom()
{
    double zoomVel = m_pidZoomVel.Update( m_tgtZoom - m_zoom, 0 );
    double currentTime = oapiGetSysTime();
    double timeDiff = currentTime - m_prevTime;
    m_prevTime = currentTime;
    m_zoom += zoomVel * timeDiff;
    m_zoom = GetMinAllowedZoom( m_zoom ); // Requiured if accidentally overdone the zoom out
}
