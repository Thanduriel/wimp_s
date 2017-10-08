# wimp_s
roguelike spaceship battle game with space-time curving weapons

## build
* clone repo
* update submodules
* run cmake on CMakeLists.txt in the root
* copy "defaultconfig.hpp" from dependencies/epsilon/include/ei to dependencies/epsilon/include and rename it to "eiconfig.hpp"
* fetch a dev lib of [SDL 1.2](https://www.libsdl.org/download-1.2.php)
* compile with toolchain of your choice(!currently only visual studio 2017 Win64 works)
