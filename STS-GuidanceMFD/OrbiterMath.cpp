//////////////////////////////////////////////////////////////////////////////////////////////
//
//	OrbiterMath.cpp - General math operation that I found useful for Orbiter programming
//
//	Copyright Chris Knestrick, 2002
//
//	For a full description of the copyright, please see the file Copyright.txt
///////////////////////////////////////////////////////////////////////////////////////////////
#include "OrbiterMath.h"

void MultiplyByMatrix(const VECTOR3 &Initial, const MATRIX3 &RotMatrix, VECTOR3 &Result)
{

	Result.x =	(Initial.x * RotMatrix.m11) 
				+ (Initial.y * RotMatrix.m12) 
				+ (Initial.z * RotMatrix.m13);
	
	Result.y =	(Initial.x * RotMatrix.m21) 
				+ (Initial.y * RotMatrix.m22) 
				+ (Initial.z * RotMatrix.m23);
	
	Result.z =	(Initial.x * RotMatrix.m31) 
				+ (Initial.y * RotMatrix.m32) 
				+ (Initial.z * RotMatrix.m33);
}

void RotateVector(const VECTOR3 &Initial, const VECTOR3 &Angles, VECTOR3 &Result)
{
	MATRIX3 RotMatrixX, RotMatrixY, RotMatrixZ;
	VECTOR3 AfterZ, AfterZY;					// Temporary variables


	GetRotMatrixX(Angles.x, RotMatrixX);
	GetRotMatrixY(Angles.y, RotMatrixY);
	GetRotMatrixZ(Angles.z, RotMatrixZ);
	
	MultiplyByMatrix(Initial, RotMatrixZ, AfterZ);
	MultiplyByMatrix(AfterZ, RotMatrixY, AfterZY);
	MultiplyByMatrix(AfterZY, RotMatrixX, Result);
}

#define LAT x
#define LONG y

// Returns the distance between two Lat/Long positions over the surface of a sphere of a given
// radius.
double CalcSphericalDistance(VECTOR3 Pos1, VECTOR3 Pos2, double Radius)
{
	double DeltaLat = Pos2.LAT - Pos1.LAT;
	double DeltaLong = Pos2.LONG - Pos1.LONG;

	double A = pow(sin(DeltaLat/2), 2) + cos(Pos1.LAT) * cos (Pos2.LAT) * pow(sin(DeltaLong/2), 2);
	double C = 2 * atan2(sqrt(A), sqrt(1 - A));
	
	return (Radius * C);
}

// Returns the bearing between two Lat/Long points on a sphere
double CalcSphericalBearing(VECTOR3 Pos1, VECTOR3 Pos2)
{
	
	double Result = atan2(sin(Pos1.LONG - Pos2.LONG) * cos(Pos2.LAT), 
					cos(Pos1.LAT) * sin(Pos2.LAT) - sin(Pos1.LAT) * cos(Pos2.LAT) * 
					cos(Pos1.LONG - Pos2.LONG));

	// Since we can't do mod with doubles...
	if (Result > (2*PI)) {
		Result -= 2*PI;
	}

	return Result;
}


void CalcOffsetLatLong(VECTOR3 Pos, VECTOR3& Result, double Distance, double Angle)
{
	double sinDeltaLong, cosDeltaLong, DeltaLong;

	Result.LAT = asin(sin(Pos.LAT) * cos(Distance) + cos(Pos.LAT) * sin(Distance) * cos(Angle));
	sinDeltaLong = (sin(Angle) * sin(Distance)) / cos(Result.LAT);
	cosDeltaLong = (cos(Distance) - sin(Pos.LAT) * sin(Result.LAT)) / 
					(cos(Pos.LAT) * cos(Result.LAT));
	DeltaLong = atan2(sinDeltaLong, cosDeltaLong);
	Result.LONG = Pos.LONG - DeltaLong + PI;

	// Mod
	if (Result.LONG > (2*PI)) {
		Result.LONG -= 2*PI;
	}

	Result.LONG -=PI;

}

// Add two angles  - the result is between 0 and 2*PI
double AddAngles(double Initial, double Angle)
{
	double Result;
	
	Result = Initial + Angle;

	if (Result > (2*PI)) {
		Result -= 2*PI;
	}

	return Result;
}