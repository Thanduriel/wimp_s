#include <fstream>
#include <algorithm>

#include "mesh.hpp"
#include "core/device.hpp"
#include "resources.hpp"
#include "utils/pathutils.hpp"

namespace Graphic {

	using namespace ei;

	RawMesh::RawMesh(const std::string& _pFile)
		: m_vertices(VertexArrayBuffer::PrimitiveType::TRIANGLE_LIST,
			{ {VertexAttribute::VEC3, 0}, {VertexAttribute::VEC3, 1}, {VertexAttribute::VEC2, 2} })
	{
		Utils::MeshLoader loader;
		loader.Load(_pFile, m_meshBounds, m_textureName, std::bind(&RawMesh::Load, this, std::placeholders::_1, std::placeholders::_2));
	}

	void RawMesh::Load(std::istream& _stream, size_t _numVertices)
	{
		using namespace Utils;
		size_t s = sizeof(Vertex) * _numVertices;
		Vertex* v = static_cast<Vertex*>(malloc(s));
		_stream.read(reinterpret_cast<char*>(v), s);

		m_vertices.GetBuffer(0)->SetData((void*&)v, (int)s);

		// sockets
		uint32_t numSockets;
		MeshLoader::Read(_stream, numSockets);

		for (; numSockets > 0; --numSockets)
		{
			std::string name;
			Vec3 pos;
			MeshLoader::Read(_stream, name);
			MeshLoader::Read(_stream, pos);
			m_sockets.emplace(name, pos);
		}
	}

	Mesh::Mesh(const std::string& _name, const std::string& _texture)
		: m_geometry(Resources::GetRawMesh(_name))
	{
		m_texture = &Resources::GetTexture(_texture != "" ? _texture : m_geometry.m_textureName);
	}

	void Mesh::Draw() const
	{
		if (m_texture) Device::SetTexture(*m_texture, 0);
		Device::DrawVertices(m_geometry.m_vertices, 0, m_geometry.m_vertices.GetNumVertices());
	}

	const ei::Vec3& Mesh::GetSocket(const std::string& _name) const
	{
		Assert(m_geometry.m_sockets.find(_name) != m_geometry.m_sockets.end(), "No socket with the name " + _name + " found.");
		return m_geometry.m_sockets.at(_name);
	}

}