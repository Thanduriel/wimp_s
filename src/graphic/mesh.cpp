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
		Utils::MeshLoader loader;
		std::string texName;
		loader.Load(_pFile, m_meshBounds, texName, std::bind(&Mesh::Load, this, std::placeholders::_1, std::placeholders::_2));
		m_texture = &Resources::GetTexture(texName);
	}


	void Mesh::Draw() const
	{
		if(m_texture) Device::SetTexture(*m_texture, 0);
		Device::DrawVertices(m_vertices, 0, m_vertices.GetNumVertices());
	}

	void Mesh::Load(std::istream& _stream, size_t _numVertices)
	{
		size_t s = sizeof(Vertex) * _numVertices;
		Vertex* v = static_cast<Vertex*>(malloc(s));
		_stream.read(reinterpret_cast<char*>(v), s);

		m_vertices.GetBuffer(0)->SetData((void*&)v, (int)s);
	}
}