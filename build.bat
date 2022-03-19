@echo off

set msbuild="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
%msbuild% -m -property:Configuration=Release

pause
