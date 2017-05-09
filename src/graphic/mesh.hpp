#pragma once

#include "core/texture.hpp"
#include "core/vertexbuffer.hpp"

namespace Graphic {

	class Mesh
	{
	public:
		Mesh();

		void Draw();
	private:
		struct Vertex
		{
			ei::Vec3 position;
			ei::Vec3 normal;
			ei::Vec2 textureCoords;
		};
	//	Texture m_texture;
		VertexArrayBuffer m_vertices;
	};
}