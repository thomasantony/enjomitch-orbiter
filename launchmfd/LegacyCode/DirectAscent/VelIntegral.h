#ifndef VELINTEGRAL_H
#define VELINTEGRAL_H

struct VelIntegral
{
  VelIntegral() { vel = 0; dvVert = 0; dvHori = 0; }
  VelIntegral( double vel, double dvVert, double dvHori )
  { this->vel = vel; this->dvVert = dvVert; this->dvHori = dvHori; }
  double dv() const { return dvVert + dvHori; };
  double vel;
  double dvVert;
  double dvHori;
};

#endif // VELINTEGRAL_H
