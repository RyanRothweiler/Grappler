@echo off


call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
set path=C:\OriginTower\OriginEngine\;%path%
cd C:\OriginTower\OriginEngine\

pushd build
cl ..\code\win32_OriginTower.cpp /link user32.lib /nologo
popd