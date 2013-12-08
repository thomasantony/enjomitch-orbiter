TransX V 2013.12.08 (Auto-Min)

Description & License
---------------------
This is a complex Orbiter Navigation tool. It is also Beta software - it may yet have a few undiscovered bugs, and probably a new version without them will come along in a few weeks.


Disclaimer
----------
This software is for use as an Orbiter addon only. It is free software, and is provided as is, without any warranty. It is for recreational use only, and its operation is not guaranteed to be continuous or accurate. You may redistribute it, but may not charge anything for it, other than a fee to cover your distribution costs only. It may crash your computer, or cause other errors to occur.

YOUR USE OF THIS SOFTWARE IS ENTIRELY AT YOUR OWN RISK. RUNNING OR COPYING THIS SOFTWARE IMPLIES ACCEPTANCE OF THE CONDITIONS OF USE ABOVE.

For further details and the licence, please see licence.txt included with this zip.


Installing
----------

Copy the enclosed dll file into the modules/plugin directory of your Orbiter installation. Select the module on the Modules tab of the Orbiter Launchpad, and activate it.


To start the module, either select the MFD from the onscreen menu, or use Shift-J to select it.



Use
---

A manual for this beta software does not yet exist. But hopefully most of you will be aware of the previous version of TransX. Here are the major changes.


A trip is now divided into multiple STAGES. You start off with just one stage.

To create more stages, first select a Target. The Target is always the first variable in the variable set. Then press the FWD button to create a new stage, and move into it.

FWD and BCK move your viewpoint between the stages you've created, and create a new stage if that's possible.

SW locks/unlocks the graph, whilst allowing you to move the variable selection as before.


Each stage has a PLAN associated with it. The MFD will normally auto-select the best plan for you, (although it is possible to change it if you have to). The plan is designed to help you plan your course through the stage.


Each Stage has three views. The first one isn't used often.
The second view contains an overall view of the stage, and has some generic variables to adjust.

The third view contains the variables associated with the currently selected plan. These change depending on which plan you select. It may sometimes also contain a different graph from view 2.


The graphs will largely be familiar to you if you've used TransX before. There are two differences.

The green orbit in a graph is now not always your ship, but can also be ANY type of trajectory that's been passed on from the previous stage.


There is one new graph type in the Slingshot plan in view 3. The yellow line represents your planned point of closest approach, and the green line your current one. Match the two together to get a perfect slingshot.

The easiest way to align them is to use prograde autopilot and linear manoevreing thrusters, whilst at a considerable distance from the planet.

Slingshots
----------

To create a slingshot,
1. Select your slingshot planet as a target, and move Forward to create a new stage.
2. Select 'Escape' as a target, and move forward again to create a new stage.

Move to view 3, and you will find TransX has created a plan for you to point the slingshot wherever you like. It's quite possible that the initial parameters require you to go below the planet's surface. Change the parameters until you're not going below the surface (Pe / Pl. Rad  is greater than 1), and you're going the way you want to.

If you then go back to the slingshot planet, you'll see your slingshot plan in yellow. Select view 3 for a graph to target it precisely.

New Features/Hacks:
------------------

By dgatsoulis:
-Added "Edge On" Graphic projection. 
An additional graph projection rotated 90° wrt the ecliptic. 
-Changed the function of Scale to View: Target.
Now when no target has been selected the view zooms in to the Major Body of the stage.
-Manoeuvre mode Target View:
A slightly bigger "X" in the crosshair view. No more "where the heck is that green little x?"
-Color of planet orbits and atmospheres changed to a lighter shade of blue.
Easier to see especially for YT videos.
-2 TransX MFDs for the price of one!
In addition to the transx.dll you get a second one named transx2.dll
The second transx uses a different color scheme (red).
In addition to the cool "TransX's evil twin" look, you get all the functionality mentioned above.
It allows you to setup a completely independent TransX plan and both plans get saved in your scenario.
Very handy for advanced mission planning , where you'd normally need a surrogate ship.


By Enjo: Closest Approach minimization (Auto-Min):
Setting a variable's adjustment setting to Auto-Min serves the following purposes:
1) You may use ++ or -- to find optimal value of this variable, which minimizes the Closest Approach
2) In Maj.Ejection plan, you may do the same as in 1), but also, you may leave other variables with Auto-Min setting, which will force them to be passively minimized while changing the Ejection Date, therefore automatically finding solutions.
3) After you've roughly found a solution in point 2), you may further minimize the Cl. App. by switching the Ej. Date to Auto-Min and pressing ++ or --



Any questions - post in the addons section of the Orbiter forum.

Happy Orbiting!

Duncan Sharpe & Steve Arch
http://orbiter.quorg.org