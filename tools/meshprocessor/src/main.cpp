#include "mesh.hpp"
#include <iostream>

int main(int argc, char** args)
{
	if (argc < 3)
	{
		std::cerr << "To few arguments provided, expected: <input_file> <output_file>";
//		return -1;
	}

	try {
		Mesh mesh("spaceship.fbx"/*args[1]*/);
		mesh.Save("spaceship"/*args[2]*/);
	}
	catch (std::string s) {
		std::cerr << s;
		return -1;
	}

	return 0;
}