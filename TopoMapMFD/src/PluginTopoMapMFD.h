#ifndef PLUGINTOPOMAPMFD_H
#define PLUGINTOPOMAPMFD_H

#include <OrbiterSDK.h>

class TopoMap;
class PluginTopoMapMFD : public oapi::Module
{
    public:
        PluginTopoMapMFD(HINSTANCE hDLL);
        virtual ~PluginTopoMapMFD();

        void SetTopoMap(TopoMap * topoMap) { m_tm = topoMap;};
        void clbkPreStep (double simt, double simdt, double mjd);
    protected:
    private:
        TopoMap * m_tm;
};

#endif // PLUGINTOPOMAPMFD_H
