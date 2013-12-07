#include <iostream>
#include <iomanip>
#include <cmath>
#include <Math/BinSearchOpti.hpp>
#include "BinSubjSphere.h"

using namespace std;
using namespace EnjoLib;

int main()
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


    return 0;
}
