#ifndef PERLIN_H_INCLUDED
#define PERLIN_H_INCLUDED

class PerlinNoise
{
public:
	double CreateNoise(double x, double freqamp[], int count);

private:
	double IntNoise(int x);
	double SmoothNoise1(int x);
	double Interpolate(double a, double b, double x);
	double InterpolatedNoise(double x);
};

#endif  // #ifndef PERLIN_H_INCLUDED