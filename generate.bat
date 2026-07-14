rem rmdir _temp /S /Q
"./tools/premake5.exe" vs2022 --os=windows
rem  "./tools/premake5.exe" gmake2 --os=linux
start ./_temp/supercoolproject.sln