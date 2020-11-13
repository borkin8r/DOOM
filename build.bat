mkdir build\win32
pushd build\win32
cl /c /DWIN32 ..\..\win32\*.c
link /SUBSYSTEM:WINDOWS .\*.obj kernel32.lib User32.lib gdi32.lib ws2_32.lib
popd

rem cl /DWIN32 /Zi /Fewin32_doom ..\..\win32\win32_doom.c ..\..\win32\win32_layer.c ..\..\win32\*.c
rem link /SUBSYSTEM:WINDOWS user32.lib gdi32.lib ws2_32.lib