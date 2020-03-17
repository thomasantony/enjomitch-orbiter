#include "RandomMath.hpp"
#include <random>

using namespace EnjoLib;

RandomMath::RandomMath()
{
    //ctor
}

RandomMath::~RandomMath()
{
    //dtor
}


void RandomMath::RandSeed(int seed) const
{
    if (seed == 0)
        srand (time(NULL));
    else
        srand (seed);
}

double RandomMath::Rand(double min, double max) const
{
    return rand() / static_cast<double>(RAND_MAX) * (max-min) + min;
}
