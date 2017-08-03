#include "meshloader.hpp"
#include "logger.hpp"

namespace Utils {

	size_t MeshLoader::ReadHeader(std::ifstream& _stream, MeshInfo& _info, std::string& _texName)
	{
		uint16_t ver;
		Read(_stream, ver);
		if (ver != FORMAT_VERSION) LOG_ERROR("Expected mesh format version "
			+ std::to_string(FORMAT_VERSION) + ", but found " + std::to_string(ver));
		uint16_t format;
		Read(_stream, format);

		Read(_stream, _texName);

		Read(_stream, _info.boundingRadius);
		Read(_stream, _info.lowerBound);
		Read(_stream, _info.upperBound);

		uint32_t numVertices;
		Read(_stream, numVertices);

		return numVertices;
	}
}