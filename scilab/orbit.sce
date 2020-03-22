clear
exec('/home/enjo/devel/orbiter/scilab/cross.sci');
exec('/home/enjo/devel/orbiter/scilab/len.sci');
//exec('E:\devel\Orbiter\scilab\cross.sci');
//exec('E:\devel\Orbiter\scilab\len.sci');

R = 6378100.0;
M = 5.9736e24;
G = 6.67300e-11;
mu = G * M;

g = mu / R^2; // Gravit. acceleration ~ 9.8

pos = [R 0 0]'; // starting point

v = sqrt(mu/R); // module of starting velocity
//v = 2000;

incl = 0 * %pi / 180; // inclination
vel = [0; cos(incl) * v; sin(incl) * v ]; // starting velocity

T = 2*%pi*R / v; // orbital period

n = cross( vel, pos ); // orbit's normal

tincr = 10; // time increments
acc = 15; // ship's acceleration in m/s^2
az = -10 * %pi / 180; // acceleration azimuth

// position vectors
outPos1 = []; // first orbit
outPos2 = []; // second orbit

points = round(T / tincr);
for j = 1:points * 2
  // gravity acting on object
  //xg = ( len(pos) - g*tincr^2/2 ) / len(pos); // set magnitude
  xg = R / len(pos); // simplification if we use v < 1st cosmic
  pos = pos * xg; // shorten the position vector as required

  n = cross( vel, pos ); // update the orbit in case velocity changed
  n = n / len(n);  // normalise
  ncrossp1 = -cross(n, pos); // find perpendicular velocity
  xv = len( vel ) / len( ncrossp1 ); // set magnitude (wrt current vel)
  //xv = v / len( ncrossp1 ); // set magnitude (wrt starting vel)
  vel = ncrossp1 * xv; // shorten the velocity vector as required

  // simulate acceleration in other direction after making 1 orbit
  // see http://www.euclideanspace.com/maths/geometry/rotations/theory/inaPlane/index.htm
  if (j > points & j < points + 60) then
    velNorm = vel / len(vel);
    vn = [velNorm n]; // plane of rotation
    v2d = vn' * vel; // cast velocity to 2D space - plane of rotation
    vadd = [ acc * tincr * sin(az); -acc * tincr * cos(az) ]; // accelerated
    v2d = v2d + vadd; // add it to 2d space velocity vector (rotate)
    vel = vn * v2d; // convert back to 3D space
    
   // param3d([pos(1) pos(1) + vel(1) * tincr], [pos(2) pos(2)+ vel(2) * tincr], [pos(3) pos(3)+vel(3) * tincr]);
  end;

  pos = pos + vel * tincr; // update position vector by velocity
  
  

  // add points to first vector if doing first orbit
  if j < points then
    outPos1 = [outPos1 pos];
  else // the second orbit
    outPos2 = [outPos2 pos];
  end;
end;

colors = outPos1(3,:) * 0;
colors = colors + 2;
param3d1(outPos1(1,:), outPos1(2,:), list(outPos1(3,:), colors));

colors = outPos2(3,:) * 0;
colors = colors + 5;
param3d1(outPos2(1,:), outPos2(2,:), list(outPos2(3,:), colors));

