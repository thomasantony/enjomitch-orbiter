#include <iostream>
#include <Math/BinSearchOpti.hpp>
#include "BinSubjSphere.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace EnjoLib;

int main()
{
    srand ( time(NULL) );
    vector<double> a {-10, -10, -10};
    vector<double> b {+10, +10, +10};
    BinSearchOpti bsearch(a, b, 0.0001);

    for (double shiftx = a.at(0)-1; shiftx < b.at(0)+1; shiftx += 1.2 )
    {
        double shifty = rand() % 10 + 1;
        double shiftz = rand() % 10 + 1;
        vector<double> shift {shiftx, shifty, shiftz};
        BinSubjSphere sph(shift);
        Result<Vector> result = bsearch.Run(sph);
        cout << ": " << result.status;
        cout << ", x = " << shift;
        cout << ", xopt = " << result.value;
        cout << ", y(xopt) = " << sph.UpdateGetValue(result.value);
        cout << endl;
    }

    return 0;
}
