#ifndef STAGE_H
#define STAGE_H

class VESSEL;

struct Stage
{
    Stage();
    Stage( double tmeco, const VESSEL * v );

    double tmeco;
    double accel;
    double isp;
};

#endif // STAGE_H
