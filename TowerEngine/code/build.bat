@echo off

set GLFWPath= C:\Users\Ryan\Documents\OriginTower\TowerEngine\code\GLFW\glfw3dll.lib 
set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4127 -wd4189 -wd4505  -wd4065 -wd4700 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib Xinput.lib opengl32.lib %GLFWPath%
set BuildVariables= -DINTERNAL=1 -DSLOW=1


call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64

pushd ..\build


REM 64-bit build
del *.pdb > NUL 2> NUL
cl %BuildVariables% %CommonCompilerFlags% ..\code\OriginTower.cpp -LD /link -incremental:no -opt:ref  opengl32.lib %GLFWPath% /PDB:origin_%random%.pdb /EXPORT:GameLoop /EXPORT:GameLoadAssets
cl %BuildVariables% %CommonCompilerFlags% ..\code\win32_OriginTower.cpp /link %CommonLinkerFlags%
popd