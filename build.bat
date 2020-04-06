mkdir win32
pushd win32
cl /Zi /Fe.\win32_doom.exe ..\linuxdoom-1.10\win32_doom.c user32.lib gdi32.lib
popd