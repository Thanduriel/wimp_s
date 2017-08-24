#include "mesh.hpp"
#include "pathutils.hpp"
#include "convexhullgen.hpp"
#include <iostream>


Mesh::Format meshFormat = Mesh::Format::Flat;
Mesh::Format boundingMeshFormat = Mesh::Format::IndexedNormal;
float epsilon = 0.05f;
bool saveBoundingMesh = true;
std::string textureName = "";

using namespace std;

void ProcessArg(const std::string& _str)
{
	if (_str == "-nobm")
	{
		saveBoundingMesh = false;
		return;
	}
	
	Mesh::Format* dst = nullptr;

	auto split = _str.find('=', 0);
	if (split == std::string::npos)
	{
		std::clog << "Ignoring unknown parameter: " + _str << endl;
		return;
	}
	std::string c = _str.substr(0, split);
	string f = _str.substr(split + 1);

	if (c == "-format" || c == "-formatb")
	{
		dst = c == "-format" ? &meshFormat : &boundingMeshFormat;

		auto it = Mesh::FORMAT_NAMES.find(f);
		if (it == Mesh::FORMAT_NAMES.end())
		{
			clog << "Unknown format: " + f + ". Using default instead." << endl;
			return;
		}
		*dst = it->second;
	}
	else if (c == "-epsilon")
	{
		epsilon = stof(f);
	}
	else if (c == "-texture")
		textureName = f;
	else std::clog << "Ignoring unknown parameter: " + _str << endl;
}

int main(int argc, char** args)
{
	if (argc < 2)
	{
		std::cerr << "To few arguments provided, expected: <input_file> <params>..." << endl;
		return -1;
	}
	std::vector<std::string> names;

	// read command line arguments
	for (int i = 1; i < argc; ++i)
	{
		if (args[i][0] == '-')
		{
			ProcessArg(args[i]);
		}
		else names.push_back(args[i]);
	}

	ConvexHullGen generator;
	try {
		for (auto& fname : names)
		{
			std::string name = PathUtils::GetName(PathUtils::CanonicalizePath(fname));

			Mesh mesh(fname);
			mesh.Save(name, meshFormat, false, textureName);

			if (saveBoundingMesh)
			{
				Mesh boundingBox = generator.Generate(mesh, epsilon);
				boundingBox.Save(name+"bm", boundingMeshFormat, true);
			}
		}
	}
	catch (std::string s) {
		std::cerr << s << endl;
		return -1;
	}

	return 0;
}