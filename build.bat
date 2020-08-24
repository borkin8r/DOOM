mkdir build\win32
pushd win32
cl /DWIN32 /Zi /Fe..\build\win32\win32_doom.exe win32_doom.c win32_layer.c doomdef.c doomstat.c d_main.c *.c user32.lib gdi32.lib ws2_32.lib
popd