# -*- coding: utf-8 -*-
"""
Created on Mon Sep 07 06:56:59 2015

@author: Szymon Ender "Enjo"
"""

import os
import sys
import zipfile
import copy
import shutil
import glob
import platform

import sys

version = 2016
#version = 2010
#version = 2006

dirPrefixLinux = '/home/enjo/devel/devel-orb/'
dirPrefixWindows = 'C:/Users/Justyna/Documents/03 SZYMON/devel/'

if platform.system() == "Linux":
    dirPrefix = dirPrefixLinux
else:
    dirPrefix = dirPrefixWindows

if version == 2016:
    srcDir = dirPrefix + "Orbiter/orbiter2016"
elif version == 2010:
    srcDir = dirPrefix + "Orbiter/orbiter100830"
elif version == 2006:
    srcDir = dirPrefix + "Orbiter/orbiter060929"
else:
    print("Version not provided!")
    sys.exit()
    
dstDir = dirPrefix + "releases-{0}".format(version)

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
        
def distribute(addons, version):
    for addon in addons:
        print (addon.name)
        try:
            pathAddon = os.path.join(dstDir, addon.name)
            for fileSrc, fileDst in zip(addon.filesSrc, addon.filesDst):
                print ("{0} -> {1}".format(fileSrc, fileDst))
                src = os.path.join(srcDir, fileSrc)
                dst = os.path.join(pathAddon, fileDst)
                shutil.copy(src, dst)
            
            for fl in glob.glob(pathAddon + "/*.zip"):
                # Remove previous zip files
                os.remove(fl)
                print("Removed " + fl)
            
            cwdPrev = os.getcwd()
            os.chdir(pathAddon)
            zipfPath = pathAddon + "-" + str(version) + ".zip"
            zipf = zipfile.ZipFile(zipfPath, 'w', zipfile.ZIP_DEFLATED)
            print("Zipping file " + zipfPath + " in " + pathAddon)
            zipdir(".", zipf)
            zipf.close()
            
            #shutil.move(zipfPath, addon.name + ".zip")
            os.chdir(cwdPrev)
            #sys.exit()
        except Exception as e:
            print(e)
            print("!!!! FAILED !!!!")
        print ("")


def zipdir(path, ziph):
    # ziph is zipfile handle
    for root, dirs, files in os.walk(path):
        for file in files:
            ziph.write(os.path.join(root, file))

        
addons = []

# Simple addons
#addon = Addon("AbsoluteKillrot-v.1.2")
#addon.AddFile1("Modules/Plugin/AbsoluteKillrot.dll")
#addons.append(copy.deepcopy(addon))

#addon = Addon("TopographicMapMFD-v.0.5")
#addon.AddFile1("Modules/Plugin/TopoMapMFD.dll")
#addons.append(copy.deepcopy(addon))

addon = Addon("BurnTimeCalcMFD-v.3.0.0")
addon.AddFile1("Modules/Plugin/BurnTimeMFD.dll")
addons.append(copy.deepcopy(addon))

addon = Addon("LaunchMFD-v.1.6.5")
addon.AddFile1("Modules/Plugin/LaunchMFD.dll")
addons.append(copy.deepcopy(addon))

addon = Addon("LaunchMFD-v.1.6.5-pl")
addon.AddFile1("Modules/Plugin/LaunchMFD_pl.dll")
addons.append(copy.deepcopy(addon))

# Complex addons
addon = Addon("TransX-2018.04.05-MMExt2")
addon.AddFile1("Modules/Plugin/TransX.dll")
addon.AddFile1("Modules/Plugin/TransX2.dll")
addons.append(copy.deepcopy(addon))

distribute(addons, version)
print("Ready!")

sys.exit()

# Libraries
addon = Addon("ModuleMessagingSDK-v.1.1")
addon.AddFile1("Modules/ModuleMessaging.dll")
addon.AddFile2("Modules/ModuleMessaging.lib", "Orbitersdk/lib/ModuleMessaging.lib")
#addons.append(copy.deepcopy(addon))

addon = Addon("ModuleMessagingExt-v1.2")
addon.AddFile1("Modules/ModuleMessagingExt.dll")
addon.AddFile2("Modules/ModuleMessagingExt.lib", "Orbitersdk/lib/ModuleMessagingExt.lib")
addons.append(copy.deepcopy(addon))

addon = Addon("HUDDrawerSDK-v.0.4")
addon.AddFile1("Modules/Plugin/HUDDrawer.dll")
addon.AddFile1("Modules/VesselHooking.dll")
addon.AddFile2("Modules/VesselHooking.lib", "Orbitersdk/lib/VesselHooking.lib")
addons.append(copy.deepcopy(addon))




