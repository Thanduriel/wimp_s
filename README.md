# wimp_s
A roguelight spaceship battle game with space-time curving weapons using OpenGL 4.
![ingame example](https://user-images.githubusercontent.com/5833421/31316630-4de61c48-ac31-11e7-93b8-cc1e309d95d6.png)
## build
First clone this repo with submodules

	git clone --recursive https://github.com/Thanduriel/wimp_s.git

Install SDL 2.0 so that it can be found by CMAKE. Alternatively
 get a [dev lib](https://www.libsdl.org/download-2.0.php) and extract its contents into `dependencies/SDL`.
Afterwards, run cmake to create the build files.

	cd wimp_s
	mkdir build
	cd build
	cmake ..

Before running it, you may have to put a copy of `SDL2.dll` into the same folder as the executable.
Note that a debug build expects to be run from inside the build folder, while a release build looks for the resources in the same directory as the executable.