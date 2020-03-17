---============INTRO=============---
This is a calculator of a true azimuth in which you must launch to have 
a desired inclination after reaching orbit (eg. ISS' inclination). 
The equations used take planets' rotation into account. Because azimuth 
changes as a function of latitude, if you swerve it's impossible to know 
which azimuth you should have using this program. It's for launch only.
Program has been written in C++ win32 so it doesn't need any additional
installs and was created with compatibility with a new multiplatform 
simulator Kosmos in mind (hail!).Target has been achieved - 
program should work under all *NIXes with the help of Wine.

---===========UPDATES============---

20.11.2006 v.1.5

I have created a graphical interface. Except some minor tweaks, 
the algorithm itself is the same (unless I've gefrikkt somthing :) ) 

27.08.2006 v.1.2

I've decided to include Moon's rotation after all. 
Sorry if not including it caused trouble :)

26.08.2006 v.1.1

1) !!Program uses brand new formulas that give...
almost the same result :) . Seriously the new formulas allowed to improve UI.
Now you enter target orbit's height instead of it's orbital velocity. 
(the OV is calculated). How would you know v=f(alt) ?

2) Added custom planet option where you can enter parameters of any body (extra sol aware),
if you're not into C++ and can't edit the source yourself to suit your needs.
Just open body's configuration file in ..\Orbiter\Config and type the values.
Instructions will be displayed as you enter them.

3) Some tweaks, including for retrograde orbits. The calculator is more informative.
Just try entering eg. "23" as an inclination and a bigger latitude. Then eg. inclination
= "162" and latitude "38".
_____________________________________

There were discussions on m6 about Shuttles' necessary azimuth when inbound to ISS,
and the suggested one was 42.4 while for the same configuration my program will output
about 42.85. The following sentence is just my speculation but it's possible that it's 
the Shuttles' autopilot which introduces an error in final inclination using this formula
or it's just a normal spaceflight phenomena which I can't explain,... or both.

Here's what I've done to test formula's correctness:
1) Launched Cape Canaveral scenario, enabled Scn. Editor and calculated azimuth using 
 latitude given from Scn. Ed. and arbitraty selected altitude (250km)
2) Paused the sim
3) Used Scn. Ed. to adjust my heading to 42.85
4) Started raising Semi-major axis using Scn. Ed. to be on 250 km altitude 
5) Using "Impulsive burn", executed "Normalized launch vector" from the calc.
6) Unpaused 

The final inclination was 51.61 instead of 51.57 (d = 0.04) but also note that eccentricity 
was 0.004, making PeA 205.9 and ApA 250 what maks me think that the impulsive burn wasn't 
purely prograde.
Screenshots:
http://www.elwico.pl/~ender-sz/img/orb/az_calculator1.png
http://www.elwico.pl/~ender-sz/img/orb/az_calculator2.png

Also check this m6 thread out:
http://www.orbitersim.com/Forum/default.aspx?g=posts&t=9464


Credits:
BEEP (basic equations) 
Urwumpe (previous equations and some essential info)
Andy44 (new equations provider)
Jerry Jon Sellers (book "Understanding Space" from where the new equations come from)
Sputnik (idea how to test the output value...)
Dcjacob (... and author of Impulsive Burn which made such test possible)
Orbiter irc chaps, especially Daver (programming help, as usual :> )

Resources that I used:
http://www.winprog.net/tutorial/
http://www.catch22.net/tuts/
http://www.foosyerdoos.fsnet.co.uk/
http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winprog/winprog/functions_in_alphabetical_order.asp

And an excellent WinDialogEditor - VC++ dialog editor alternative, 
modified by Daver, to be compatible with Wine.


/***************************************************************************
 *   Copyright (C) 2006 by Szymon "Enjo" Ender  						   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/