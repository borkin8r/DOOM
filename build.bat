mkdir build/win32
pushd build/win32
cl /Zi /Fe.\win32_doom.exe ..\..\win32\win32_doom.c user32.lib gdi32.lib
popd