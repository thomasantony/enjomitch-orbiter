clear
r = 5;

offset = 0;
//offset = %pi/2;
// geographic coordinates
// 1 - starting point
// 2 - ending point

// latitude should be in range (-%pi/2, %pi/2)
// if displaying a satellite, lat1 is inclination
// divide the path into 2 steps - from 1 to 2 and from 2 to 3
// Increases precision greatly
lat1 = 80.57 * %pi / 180; 
//lat1 = 0.1 * %pi / 180; 
lat2 = 0;
//lat1 = 0.28 * %pi / 2; 
lat3 = -lat1; // opposite side of the panet (can be changed)

// longitude should be in range (-%pi, %pi)
lon1 = 0 * %pi / 180;
lon2 = lon1 + %pi/2;
lon3 = lon2 + %pi / 2; // opposite side of the panet (+ PI) (can be changed)
// converting to spherical coordinates

// theta should be in range of (0, %pi)
theta1 = lat1 + %pi/2;
theta2 = lat2 + %pi/2;
theta3 = lat3 + %pi/2;

// phi should be in range of (0, %pi) (??)
phi1 = 0;
phi2 = lon2 - lon1;
phi3 = lon3 - lon1;



// convert to cartesian coordinates
x1 = [0, r*sin(theta1)*cos(phi1)];
y1 = [0, r*sin(theta1)*sin(phi1)];
z1 = [0, r*cos(theta1)];

x2 = [0, r*sin(theta2)*cos(phi2)];
y2 = [0, r*sin(theta2)*sin(phi2)];
z2 = [0, r*cos(theta2)];

x3 = [0, r*sin(theta3)*cos(phi3)];
y3 = [0, r*sin(theta3)*sin(phi3)];
z3 = [0, r*cos(theta3)];

// create vectors for easy calculations
v1 = [x1(2), y1(2), z1(2)];
v2 = [x2(2), y2(2), z2(2)];
v3 = [x3(2), y3(2), z3(2)];



// actual path
vTheta12 = []; // 1st quart
vPhi12 = []; 
vTheta23 = []; // 2nd quart
vPhi23 = []; 

// mirrored path
vPhi34 = [];

vPhi41 = [];

fixedStep = 0.04;
i = -fixedStep;
step = fixedStep;

// here, we'll be changing i from 0 to 1 
// to properly scale a resulting vector below...
while (i <= 1 - step)
  scf(0);
  
  i = i + step;
  
  // just a variable precision
  //if i > 0.30 & i < 0.70
//    step = fixedStep/2;
//  else
//    step = fixedStep;
//  end

  // the meatball equation of this approach:
  // From v1, draw a vector to v2 and multiply it from 0 to 1
  // so that we have many "straight line" points between p1 and 2
  // on a plane defined by p1, p2 and origin of the planet (0,0,0)
  v12 = v1 + i * (v2 - v1);
  v23 = v2 + i * (v3 - v2);
  
  
  // check the length of the new point's vector
  r12 = sqrt(v12(1)^2 + v12(2)^2 + v12(3)^2);
  r23 = sqrt(v23(1)^2 + v23(2)^2 + v23(3)^2);
  // make the vector as long as planet's radius
  x121 = [0, v12(1)];
  y121 = [0, v12(2)];
  z121 = [0, v12(3)];
 
  v12tmp = v12;
  v23tmp = v23;

  v12 = v12 * r / r12;
  v23 = v23 * r / r23;
//  rtest = sqrt(vtest(1)^2 + vtest(2)^2 + vtest(3)^2);
  // for plotting
  x12 = [0, v12(1)];
  y12 = [0, v12(2)];
  z12 = [0, v12(3)];
  
  x23 = [0, v23(1)];
  y23 = [0, v23(2)];
  z23 = [0, v23(3)];
  
  param3d(x12, y12, z12);
  param3d(x23, y23, z23);
//  param3d([v1(1) v2(1)], [v1(2) v2(2)], [v1(3) v2(3)]);
  //param3d([v2(1) v3(1)], [v2(2) v3(2)], [v2(3) v3(3)]);
  
  // convert to spherical
  // r - already calculated
  theta12 = acos(v12(3) / r);
  phi12 = atan(v12(2) / v12(1));
  
  theta23 = acos(v23(3) / r);
  phi23 = atan(v23(2) / v23(1));
  
  // ensure proper range
  if phi12 < 0
    phi12 = phi12 + %pi;
  end

  if phi23 < 0
    phi23 = phi23 + %pi;
  end

  // add new calculations to a vector
  vTheta12 = [vTheta12, theta12];
  vPhi12 = [vPhi12, phi12];
  vTheta23 = [vTheta23, theta23];
  vPhi23 = [vPhi23, phi23];
  
  // mirror path (on the other side of planet)
  phi34 = phi12 + %pi;
  vPhi34 = [vPhi34, phi34];
  phi41 = phi23 + %pi;
  vPhi41 = [vPhi41, phi41];
end

// draw the final line with a different color
//plot2d(x12, y12);
param3d1(x12, y12, list(z12, [3,2]));
param3d1(x23, y23, list(z23, [3,2]));
param3d1(x1, y1, list(z1, [3,2]));
  
//vTheta = [vTheta, theta2];
//vPhi = [vPhi, phi2];
// convert back to geographical

vTheta12 = vTheta12 - %pi/2;
vTheta23 = vTheta23 - %pi/2;
// use the initial longitude 
vPhi12 = vPhi12 + lon1 + offset;
vPhi23 = vPhi23 + lon1 + offset;

vPhi34 = vPhi34 + lon1 + offset;
vPhi41 = vPhi41 + lon1 + offset;

// ensure proper ranges
s = size(vPhi12);
for i = 1:s(2)
if vPhi12(i) >= %pi
    vPhi12(i) = vPhi12(i) - 2*%pi;
end
if vPhi23(i) >= %pi
    vPhi23(i) = vPhi23(i) - 2*%pi;
end
if vPhi34(i) >= %pi
    vPhi34(i) = vPhi34(i) - 2*%pi;
end
if vPhi41(i) >= %pi
    vPhi41(i) = vPhi41(i) - 2*%pi;
end

end


scf(1);
// draw equator
plot2d([-%pi %pi], [0 0]);
// draw the actual trajectory from point 1 to 2
plot2d(vPhi12 ,  vTheta12, style=1,  rect=[-%pi, -%pi/2, %pi, %pi/2]);
plot2d(vPhi23 ,  vTheta23, style=1,  rect=[-%pi, -%pi/2, %pi, %pi/2]);
// draw mirror on the other side of planet
plot2d(vPhi34, -vTheta12, style=-2, rect=[-%pi, -%pi/2, %pi, %pi/2]);
plot2d(vPhi41, -vTheta23, style=-2, rect=[-%pi, -%pi/2, %pi, %pi/2]);

// just a test of distance calculation
R = 6378;
D = acos( sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1) ) * R;
