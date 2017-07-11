#pragma once

#include "core/texture.hpp"
#include "core/vertexbuffer.hpp"

namespace Graphic {

	class Mesh
	{
	public:
		Mesh();
		Mesh(const std::string& _pFile);

		void Draw() const;

		// Returns the radius of the minimum sized sphere
		// positioned at point 0, that contains all vertices.
		float GetBoundingRadius() const { return m_boundingRadius; }
		const ei::Vec3& GetLowerBound() const { return m_lowerBound; }
		const ei::Vec3& GetUpperBound() const { return m_upperBound; }
	private:
		struct Vertex
		{
			ei::Vec3 position;
			ei::Vec3 normal;
			ei::Vec2 textureCoords;
		};
		void Load(const std::string& _file);
		void Mesh::ComputeBoundingValues(const Vertex* _vertices, size_t _numVertices);

		Texture* m_texture;
		VertexArrayBuffer m_vertices;

		float m_boundingRadius;
		ei::Vec3 m_lowerBound;
		ei::Vec3 m_upperBound;
	};
}