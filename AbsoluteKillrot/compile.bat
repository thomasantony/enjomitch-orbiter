D:\WINDDK\3790.1830\bin\x86\cl /O2 /GL  /I "D:\WINDDK\3790.1830\inc\crt" /I "D:\WINDDK\3790.1830\inc\wxp" /I "F:\cosmo\dev\orbitersdk\include" /I "D:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\include" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /D "_CRT_SECURE_NO_DEPRECATE" /D "_WINDLL" /FD /EHsc /MD /Fo"objects\\" /W3 /nologo /c /Zi /TP AbsoluteKillrot.cpp

D:\WINDDK\3790.1830\bin\x86\link /OUT:".\objects\AbsoluteKillrot.dll" /INCREMENTAL:NO /LIBPATH:"D:\WINDDK\3790.1830\lib\crt\i386" /LIBPATH:"D:\WINDDK\3790.1830\lib\wxp\i386" /LIBPATH:"D:\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Lib" /LIBPATH:"D:\dev\Microsoft Visual C++ Toolkit 2003\lib"  /LIBPATH:"F:\cosmo\dev\orbitersdk\lib" /LIBPATH:"D:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\Lib" /DLL /NODEFAULTLIB:"msvcirt.lib" /NODEFAULTLIB:"msvcprt.lib" /SUBSYSTEM:WINDOWS /OPT:REF /OPT:ICF /LTCG /MACHINE:X86 kernel32.lib user32.lib gdi32.lib orbiter.lib orbitersdk.lib  ".\objects\AbsoluteKillrot.obj"
