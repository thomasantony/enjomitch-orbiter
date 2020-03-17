wine "C:\winddk\3790.1830\bin\x86\cl" /EHsc /c /I"C:\winddk\3790.1830\inc\w2k" /I"C:\winddk\3790.1830\inc\crt" /I"C:\winddk\3790.1830\inc\wnet" API_Form0.cpp azimuth.cpp

wine "C:\winddk\3790.1830\bin\x86\rc" /I"C:\winddk\3790.1830\inc\w2k" Form0.rc

wine "C:\winddk\3790.1830\bin\x86\link" API_Form0.obj azimuth.obj Form0.RES /LIBPATH:"C:\winddk\3790.1830\lib\w2k\i386"  /LIBPATH:"C:\winddk\3790.1830\lib\w2k\i386" gdi32.lib /LIBPATH:"C:\winddk\3790.1830\lib\crt\i386" /LIBPATH:"C:\winddk\3790.1830\lib" kernel32.lib user32.lib comctl32.lib bufferoverflowU.lib  /out:azimuth-1.5-pl.exe
