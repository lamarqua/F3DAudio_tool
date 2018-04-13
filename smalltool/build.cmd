@echo off

rem Z7 or Zi for debug info
set COMPILER_FLAGS=-MTd -nologo -Gm- -FC -Zi

set LINK_PATH="C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86\"

set INCLUDE=C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include;E:\Home\dev\FACT\src;%INCLUDE%
rem echo %INCLUDE%

rem echo ==COPYING FAUDIO==
echo ...
cp E:\Home\dev\FACT\visualc\Debug\FAudio.lib .
cp E:\Home\dev\FACT\visualc\Debug\FAudio.dll .
cp E:\Home\dev\FACT\visualc\Debug\FAudio.pdb .


echo ==COMPILING==

cl.exe %COMPILER_FLAGS% smalltool.cpp /link /libpath:%LINK_PATH%  x3daudio.lib Ole32.lib FAudio.lib

if "%ERRORLEVEL%"=="0" goto :end

echo ==ERROR==
exit %ERRORLEVEL%

:end
echo ==DONE==
