#pragma once

#include <fstream>
#include <string>
#include "ei/vector.hpp"

namespace Utils {

	struct MeshInfo
	{
		ei::Vec3 lowerBound;
		ei::Vec3 upperBound;
		float boundingRadius;
	};

	class MeshLoader
	{
	public:
		static const int FORMAT_VERSION = 3;

		// general purpose binary read from file
		template<typename T>
		static void Read(std::ifstream& _stream, T& _data)
		{
			_stream.read(reinterpret_cast<char*>(&_data), sizeof(T));
		}

		template<typename Fn>
		void Load(const std::string& _fileName, MeshInfo& _info, std::string& _texName, Fn _readData)
		{
			std::ifstream file(_fileName, std::ios::binary);
			size_t numVertices = ReadHeader(file, _info, _texName);

			_readData(file, numVertices);
		}

	private:
		size_t ReadHeader(std::ifstream& _stream, MeshInfo& _info, std::string& _texName);
	};
}