#pragma once

#include "ei/vector.hpp"
#include "graphic/core/vertexbuffer.hpp"

namespace Graphic {

	class SunRenderer
	{
	public:
		SunRenderer();

		// creates a sun with the given properties.
		void Add(const ei::Vec3& _position, float _radius);

		void Draw(const ei::Mat4x4& _worldViewProjection);
	private:
		VertexArrayBuffer m_vertices;
	};
}