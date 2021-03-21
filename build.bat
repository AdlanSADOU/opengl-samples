@echo off

set CF=/std:c++latest /DEBUG /EHsc /MDd /Zi /MP

set VENDOR=..\vendor

set SDL2_INC=/I %VENDOR%\SDL2\include
set GLEW_INC=/I %VENDOR%\GLEW\include
set GLM_INC=/I %VENDOR%\GLM
set ASSIMP_INC=/I %VENDOR%\Assimp\include

set LIBPATH=/LIBPATH:..\vendor

set SDL2_LIBRARY_PATH=%LIBPATH%\SDL2\lib\x64
set GLEW_LIBRARY_PATH=%LIBPATH%\GLEW\lib\Release\x64
set ASSIMP_LIBRARY_PATH=%LIBPATH%\Assimp\lib\x64

set LIBS=user32.lib shell32.lib OpenGL32.lib GLu32.lib SDL2main.lib SDL2.lib glew32.lib assimp.lib

set LF=/link /SUBSYSTEM:CONSOLE %SDL2_LIBRARY_PATH% %GLEW_LIBRARY_PATH% %ASSIMP_LIBRARY_PATH% %LIBS%

mkdir build
pushd build

cl %CF% /Fe:prog.exe ..\src\*.cpp %SDL2_INC%  %GLEW_INC%  %GLM_INC%  %ASSIMP_INC% %LF%

popd