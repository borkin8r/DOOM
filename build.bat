mkdir build\win32
pushd build\win32
cl /DWIN32 /Zi /Fewin32_doom ..\..\win32\win32_doom.c ..\..\win32\win32_layer.c ..\..\win32\*.c user32.lib gdi32.lib ws2_32.lib
popd