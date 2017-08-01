#pragma once

#include <unordered_map>

#include "core/texture.hpp"
#include "core/vertexbuffer.hpp"
#include "utils/meshloader.hpp"

namespace Graphic {

	class Mesh
	{
	public:
		Mesh();
		Mesh(const std::string& _pFile);

		void Draw() const;

		// Returns the radius of the minimum sized sphere
		// positioned at point 0, that contains all vertices.
		const Utils::MeshInfo& GetMeshBounds() const { return m_meshBounds; }

		const ei::Vec3& GetSocket(const std::string& _name) const;
	private:
		struct Vertex
		{
			ei::Vec3 position;
			ei::Vec3 normal;
			ei::Vec2 textureCoords;
		};
		void Load(std::istream& _stream, size_t _numVertices);

		Texture* m_texture;
		VertexArrayBuffer m_vertices;
		std::unordered_map<std::string, ei::Vec3> m_sockets;


		Utils::MeshInfo m_meshBounds;

		friend class Resources; // to allow preloading
	};
}