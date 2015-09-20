# -*- coding: utf-8 -*-
"""
Created on Mon Sep 07 06:56:59 2015

@author: Szymon Ender "Enjo"
"""

import os
import copy
import shutil

srcDir = "C:/Users/Justyna/Documents/03 SZYMON/devel/orbiter/orbiterBETA"
dstDir = "C:/Users/Justyna/Documents/03 SZYMON/devel/releases-beta/beta-14"
#srcDir = "C:/Users/Justyna/Documents/03 SZYMON/devel/orbiter/orbiter100830"
#dstDir = "C:/Users/Justyna/Documents/03 SZYMON/devel/releases"

class Addon:
    def __init__(self, name):
        self.name = name
        self.filesSrc = []
        self.filesDst = []
        
    def AddFile1(self, src):
        "Source same as dest"
        self.AddFile2(src, src)
        
    def AddFile2(self, src, dst):
        "Source different than dest"
        self.filesSrc.append(src)
        self.filesDst.append(dst)
        
def disp(addons):
    for addon in addons:
        print addon.name
        for fileSrc, fileDst in zip(addon.filesSrc, addon.filesDst):
            print "{0} -> {1}".format(fileSrc, fileDst)
            src = os.path.join(srcDir, fileSrc)
            dst = os.path.join(dstDir, addon.name, fileDst)
            shutil.copy(src, dst)
        print ""

        
addons = []

# Simple addons
#addon = Addon("AbsoluteKillrot-v.1.2")
#addon.AddFile1("Modules/Plugin/AbsoluteKillrot.dll")
#addons.append(copy.deepcopy(addon))

addon = Addon("BurnTimeCalcMFD-v.2.7")
addon.AddFile1("Modules/Plugin/BurnTimeMFD.dll")
#addons.append(copy.deepcopy(addon))

addon = Addon("LaunchMFD-v.1.6.3-2010")
addon.AddFile1("Modules/Plugin/LaunchMFD.dll")
#addons.append(copy.deepcopy(addon))

# Complex addons
addon = Addon("TransX-2014.09.20-Brent")
addon.AddFile1("Modules/Plugin/TransX.dll")
addon.AddFile1("Modules/Plugin/TransX2.dll")
addons.append(copy.deepcopy(addon))

# Libraries
addon = Addon("ModuleMessagingSDK-v.1.1")
addon.AddFile1("Modules/ModuleMessaging.dll")
addon.AddFile2("Modules/ModuleMessaging.lib", "Orbitersdk/lib/ModuleMessaging.lib")
#addons.append(copy.deepcopy(addon))

addon = Addon("HUDDrawerSDK-v.0.3")
addon.AddFile1("Modules/Plugin/HUDDrawer.dll")
addon.AddFile1("Modules/VesselHooking.dll")
addon.AddFile2("Modules/VesselHooking.lib", "Orbitersdk/lib/VesselHooking.lib")
#addons.append(copy.deepcopy(addon))

disp(addons)


