// Copyright 2015 by "Topper" - Original author: concept, calculations
// Copyright 2015 by Szymon Ender "Enjo" (http://www.enderspace.de) - Co-developer: optimizations, putting it together
// Released under the terms of the LGPL v3: http://www.gnu.org/licenses/lgpl.txt

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
