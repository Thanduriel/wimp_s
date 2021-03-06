#pragma once

#include <ei/vector.hpp>
#include "graphic/core/vertexbuffer.hpp"
#include "utils/color.hpp"

namespace Graphic {
	class LineRenderer
	{
	public:
		LineRenderer(const Utils::Color32F& _color = Utils::Color32F(1.f, 1.f, 1.f),
			float _thickness = 0.3f);

		// Add a line to the set.
		void Add(const ei::Vec3& _begin, const ei::Vec3& _end);
		// Removes the line added last.
		void Remove();

		void SetLinesShown(int _num) { m_numVertices = _num * 2; }
		void SetColor(const Utils::Color32F& _color) { m_color = _color; }

		void Draw(const ei::Mat4x4& _worldViewProjection);
	private:
		struct Vertex
		{
			ei::Vec3 position;
		};

		float m_thickness;
		Utils::Color32F m_color;
		Graphic::VertexArrayBuffer m_lineVertices;
		int m_numVertices;
	};
}