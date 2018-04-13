call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64

cp "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Developer Runtime\x86\X3DAudioD1_7.dll" .

mv X3DAudioD1_7.dll X3DAudio1_7.dll 

cp "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Extras\Symbols\retail\x86\dll\X3DAudioD1_7.pdb" .

mv X3DAudioD1_7.pdb X3DAudio1_7.pdb

cp "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Lib\x86\X3DAudio.lib" .

cp "E:\Home\dev\SDL2\lib\x86\SDL2.lib" .
cp "E:\Home\dev\SDL2\lib\x86\SDL2.dll" .
