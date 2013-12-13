#ifndef BODYPROVIDER_H
#define BODYPROVIDER_H

#include <vector>
#include <map>
#include <Orbitersdk.h>

struct GBODY{
	OBJHANDLE bodyhandle;
	GBODY *parent, *next, *previous; // next and previous bodies orbiting same body
	std::vector <GBODY*> satellites;
	double soisize2;
	double gravbodyratio2;
	double mass;
};

class BodyProvider
{
    public:
        BodyProvider();
        virtual ~BodyProvider();
        void InitialiseSolarSystem();
        GBODY * GetSun();
        GBODY * GetBody(OBJHANDLE handle);
    protected:
    private:
        void DeleteGBody(GBODY *body);
        GBODY *sun; // a tree of the sun, its satellites, the satellites' satellites etc.
        GBODY * bodyCached;
        OBJHANDLE handleCached;
        std::map<OBJHANDLE, GBODY*> bodyMap;
};

#endif // BODYPROVIDER_H
