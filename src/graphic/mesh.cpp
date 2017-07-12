#include <fstream>
#include <algorithm>

#include "mesh.hpp"
#include "core/device.hpp"
#include "resources.hpp"
#include "utils/pathutils.hpp"

namespace Graphic {

	using namespace ei;

	
	Mesh::Mesh(const std::string& _pFile)
		: m_vertices(VertexArrayBuffer::PrimitiveType::TRIANGLE_LIST,
		{ {VertexAttribute::VEC3, 0}, {VertexAttribute::VEC3, 1}, {VertexAttribute::VEC2, 2} })
		,m_texture(nullptr)
	{
		Load(_pFile);

																						   // compute size properties
	//	ComputeBoundingValues(mesh->mVertices, mesh->mNumVertices);

		//m_vertices.GetBuffer(0)->SetData((void*&)vertex, 36 * sizeof(Vertex));
	/*	Vertex* vertex = (Vertex*)malloc(3 * sizeof(Vertex));
		vertex[1].position = Vec3(-15.0f, 0.0f, -15.0f);
		vertex[0].position = Vec3(15.0f, 0.0f, -15.0f);
		vertex[2].position = Vec3(0.0f, 0.0f, 15.0f);
		vertex[0].normal = Vec3(0.f, 1.f, 0.f);
		vertex[1].normal = Vec3(0.f, 1.f, 0.f);
		vertex[2].normal = Vec3(0.f, 1.f, 0.f);

		m_vertices.GetBuffer(0)->SetData((void*&)vertex, 3 * sizeof(Vertex));*/


	}


	void Mesh::Draw() const
	{
		if(m_texture) Device::SetTexture(*m_texture, 0);
		Device::DrawVertices(m_vertices, 0, m_vertices.GetNumVertices());
	}

	template<typename T>
	void read(std::ifstream& _stream, T& _data)
	{
		_stream.read(reinterpret_cast<char*>(&_data), sizeof(T));
	}

	const int FORMAT_VERSION = 2;

	void Mesh::Load(const std::string& _file)
	{
		std::ifstream file(_file, std::ios::binary);

		uint16_t ver;
		read(file, ver);
		if (ver != FORMAT_VERSION) LOG_ERROR("Expected mesh format version " 
			+ std::to_string(FORMAT_VERSION) + ", but found " + std::to_string(ver));
		uint16_t format;
		read(file, format);
		if (format != 1) LOG_ERROR("Currently only Format::Flat is supported.");


		std::string texName;
		file >> texName;
		//discard ' '
		uint32_t numVertices;
		file.read(reinterpret_cast<char*>(&numVertices), 1);

		read(file, m_boundingRadius);
		read(file, m_lowerBound);
		read(file, m_upperBound);
		file.read(reinterpret_cast<char*>(&numVertices), 4);;
		size_t s = sizeof(Vertex) * numVertices;
		Vertex* v = static_cast<Vertex*>(malloc(s));
		file.read(reinterpret_cast<char*>(v), s);

		m_vertices.GetBuffer(0)->SetData((void*&)v, (int)s);
		m_texture = &Resources::GetTexture(texName);
	}
}