# wimp_s
roguelight spaceship battle game with space-time curving weapons using OpenGL 4
![ingame example](https://user-images.githubusercontent.com/5833421/31316630-4de61c48-ac31-11e7-93b8-cc1e309d95d6.png)
## build
* clone repo
* update submodules
* run cmake on CMakeLists.txt in the root
* copy "defaultconfig.hpp" from dependencies/epsilon/include/ei to dependencies/epsilon/include and rename it to "eiconfig.hpp"
* get a dev lib of [SDL 1.2](https://www.libsdl.org/download-1.2.php) and put it into dependencies
* compile with toolchain of your choice(!currently only visual studio 2017 Win64 works)
