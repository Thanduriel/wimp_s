#include "mesh.hpp"
#include "core/device.hpp"

namespace Graphic {

	using namespace ei;

	Mesh::Mesh()
		: m_vertices(VertexArrayBuffer::PrimitiveType::TRIANGLE_LIST,
		{ {VertexAttribute::VEC3, 0}, {VertexAttribute::VEC3, 1}, {VertexAttribute::VEC2, 2} })
	{
//		m_vertices.SetNumVertices(3);

		Vertex* vertex = (Vertex*)malloc(6 * sizeof(Vertex));
		vertex[0].position = Vec3(-1.f, -1.f, 0.f);
		vertex[1].position = Vec3(1.f, -1.f, 0.f);
		vertex[2].position = Vec3(0.f, 1.f, 0.f);

		m_vertices.GetBuffer(0)->SetData((void*&)vertex, 3 * sizeof(Vertex));
	}

	void Mesh::Draw()
	{
		Device::DrawVertices(m_vertices, 0, m_vertices.GetNumVertices());
	}
}