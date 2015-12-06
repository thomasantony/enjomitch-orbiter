#ifndef PLUGINTOPOMAPMFD_H
#define PLUGINTOPOMAPMFD_H

#include <OrbiterSDK.h>

class PluginTopoMapMFD : public oapi::Module
{
    public:
        PluginTopoMapMFD(HINSTANCE hDLL );
        virtual ~PluginTopoMapMFD();
    protected:
    private:
};

#endif // PLUGINTOPOMAPMFD_H
