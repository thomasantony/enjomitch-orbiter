t = 486;
mfull = 24500;
mflow = 7;
thrust = 280000;
g = 9.81;
a = thrust / ( mfull - mflow * t ) - g;
a0 = thrust / ( mfull ) - g;
s = a * t^2 / 2 * (1 + a/g );

s2 = a * t^2 / 2 * (1 + a/g );


a = thrust / ( mfull - mflow * t )
v = a* t

t = mfull * v / ( thrust + mflow*v)
