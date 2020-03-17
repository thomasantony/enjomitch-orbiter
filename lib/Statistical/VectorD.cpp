#include "VectorD.hpp"

using namespace EnjoLib;

VectorD::VectorD( const std::vector<double> & init )
: VectorTpl(init)
{}

VectorD::VectorD( const std::vector<bool> & init )
: VectorTpl(init)
{}

VectorD::VectorD( int n )
: VectorTpl(n)
{}

VectorD::VectorD(){}
VectorD::~VectorD(){}
