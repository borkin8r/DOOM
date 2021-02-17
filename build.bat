mkdir .\build\win32
pushd .\build\win32
cl -Zi /DWIN32 ..\..\win32\*.c /link /DEBUG /OUT:doom.exe Kernel32.lib User32.lib gdi32.lib Ws2_32.lib Shell32.lib Gdi32.lib
popd
