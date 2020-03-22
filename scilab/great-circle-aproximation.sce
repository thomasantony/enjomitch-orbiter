[ma i] = max(y)
[mi j] = min(y)
Tpi = x(j-1) - x(i);
arg = [0:0.01:Tpi];

newy = ma*sin(arg / Tpi * %pi);
arg2 = arg + Tpi;
newy2 = ma*sin(arg2 / Tpi * %pi);

plot(x, y);
plot(arg,newy);
plot(arg2,newy2);

