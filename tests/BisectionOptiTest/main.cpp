#include <iostream>
#include <iomanip>
#include <cmath>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <Math/BinSearchOpti.hpp>
#include <Math/BinSearchOpti2D.hpp>
#include <Systems/Point.hpp>
#include "BinSubjSphere2D.h"
#include "BinSubjSphere.h"

using namespace std;
using namespace EnjoLib;

void test2D()
{
    srand (time(NULL));
    Point a(-8, -8);
    Point b(+8, +8);
    BinSearchOpti2D bsearch(a, b, 0.0001);

    for (double shiftx = a.x-1; shiftx < b.x+1; shiftx += 0.834 )
    {
        double shifty = (rand() % 8 + 1) * 0.93;
        Point shift(shiftx, shifty);
        //Point shift(0.6, 2);
        BinSubjSphere2D sph(shift);
        Result<Point> result = bsearch.Run(sph);
        cout << setprecision(4) << fixed;
        cout << "Status: " << result.status;
        cout << ", x = " << shift.x << "," << shift.y;
        cout << ", xopt = " << result.value.x << "," << result.value.y;
        //cout << ", xoptDif = " << fabs(result.value-shift);
        cout << ", y(xopt) = " << sph.UpdateGetValue(result.value);
        cout << endl;
    }
}

void test1D()
{
    double a = -8;
    double b = +8;
    BinSearchOpti bsearch(a, b, 0.0001);

    for (double shift = a-1; shift < b+1; shift += 0.8 )
    {
        BinSubjSphere sph(shift);
        Result<double> result = bsearch.Run(sph);
        cout << setprecision(4) << fixed;
        cout << "Status: " << result.status;
        cout << ", x = " << shift;
        cout << ", xopt = " << result.value;
        cout << ", xoptDif = " << fabs(result.value-shift);
        cout << ", y(xopt) = " << sph.UpdateGetValue(result.value);
        cout << endl;
    }
}

int main()
{
    //test1D();
    test2D();

    return 0;
}
