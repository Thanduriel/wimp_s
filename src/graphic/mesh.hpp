#pragma once

#include <unordered_map>

#include "core/texture.hpp"
#include "core/vertexbuffer.hpp"
#include "utils/meshloader.hpp"

namespace Graphic {

	class RawMesh
	{
	public:
		RawMesh(const std::string& _pFile);
		struct Vertex
		{
			ei::Vec3 position;
			ei::Vec3 normal;
			ei::Vec2 textureCoords;
		};
		void Load(std::istream& _stream, size_t _numVertices);

		VertexArrayBuffer m_vertices;
		std::unordered_map<std::string, ei::Vec3> m_sockets;
		std::string m_textureName; //< the texture that is linked in the mesh file

		Utils::MeshInfo m_meshBounds;
	};


	class Mesh
	{
	public:
		Mesh(const std::string& _pFile, const std::string& _texture = "");

		void Draw() const;

		// Returns the radius of the minimum sized sphere
		// positioned at point 0, that contains all vertices.
		const Utils::MeshInfo& GetMeshBounds() const { return m_geometry.m_meshBounds; }

		const ei::Vec3& GetSocket(const std::string& _name) const;
	private:
		const RawMesh& m_geometry;
		Texture* m_texture;

		friend class Resources; // to allow preloading
	};
}