mass_earth=5.973698968e+24;
G=6.67259e-11;
mi=mass_earth*G
In=51.57;
Lo=28.627;
a=180/%pi;
b=%pi/180;
R=6.371e6;
alt=150e3;
r=R+alt
vg=465*cos(Lo*b);
Vg=[0 vg]
aux_angle= asin( cos(In*b) / cos(Lo*b))*a
vsn=sqrt(mi/r)*cos(aux_angle*b);
vse=sqrt(mi/r)*sin(aux_angle*b);
Vs=[vsn vse]
Vl=Vs-Vg
vnorm=sqrt(Vl(1)^2+Vl(2)^2)
az=atan(Vl(2)/Vl(1))*a
