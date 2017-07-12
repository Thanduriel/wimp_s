#include "mesh.hpp"
#include "pathutils.hpp"
#include "convexhullgen.hpp"
#include <iostream>

int main(int argc, char** args)
{
	if (argc < 2)
	{
		std::cerr << "To few arguments provided, expected: <input_file> <opt output_file>";
	//	return -1;
	}
	std::vector<std::string> names;
	names.push_back("spaceship.fbx");
	names.push_back("testrocket.fbx");

	for (int i = 1; i < argc; ++i)
		names.push_back(args[i]);

	ConvexHullGen generator;
	try {
		for (auto& fname : names)
		{
			std::string name = PathUtils::GetName(PathUtils::CanonicalizePath(fname));

			Mesh mesh(fname);
			Mesh boundingBox = generator.Generate(mesh);
			mesh.Save(name);
			boundingBox.Save(name, Mesh::Format::Indexed);
		}
	}
	catch (std::string s) {
		std::cerr << s;
		return -1;
	}

	return 0;
}