@echo off
setlocal EnableDelayedExpansion

set SRC=c:\Program Files (x86)\Orbiter\_Resources\Textures
set DST=.\Textures
set JUNCTION=c:\Program Files (x86)\Orbiter\_Resources\junction.exe

for /F "tokens=*" %%d in ('dir /b /a:d "%SRC%"') do (
  set NAME=%%d
  if "!NAME:~-4!" neq ".old" (
    echo.  %%d
    if exist "%DST%/%%d" rmdir /S /Q "%DST%/%%d"
    "%JUNCTION%" "%DST%\%%d" "%SRC%\%%d" > nul
  )
)
endlocal
