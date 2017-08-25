#include <iostream>

#include "convexhullgen.hpp"
#include "QuickHull.hpp"

void ConvexHullGen::Set(Mesh& _mesh)
{
	m_mesh = &_mesh;
	m_points.clear();
	m_points.reserve(_mesh.m_vertices.size());
	for (auto& v : m_mesh->m_vertices)
		m_points.push_back(v.position);
}

Mesh ConvexHullGen::Generate(Mesh& _mesh, float _epsilon, bool _ccw)
{
	Set(_mesh);
	using namespace quickhull;
	QuickHull<float> qh;

	auto hull = qh.getConvexHull(reinterpret_cast<float*>(&m_points.front()), m_points.size(), _ccw, false, _epsilon);
	auto& vertices = hull.getVertexBuffer();
	auto& indices = hull.getIndexBuffer();

	std::clog << "Generated bounding mesh with " + std::to_string(vertices.size())
		+ " vertices and " + std::to_string(indices.size() / 3) + " faces." << std::endl;

	Mesh mesh;
	mesh.m_vertices.resize(vertices.size());
	for(size_t i = 0; i < vertices.size(); ++i)
	{
		mesh.m_vertices[i].position = vertices[i];
	}

	mesh.m_faces.resize(indices.size() / 3);
	mesh.m_normals.resize(indices.size());
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		mesh.m_faces[i / 3] = Vec<int, 3>((int)indices[i], (int)indices[i+1], (int)indices[i+2]);
		// calculate face normals
		auto l1 = vertices[indices[i]] - vertices[indices[i + 1]];
		auto l2 = vertices[indices[i]] - vertices[indices[i + 2]];
		Vec3 n; n = l1.crossProduct(l2);
		mesh.m_normals[i / 3] = n;

		mesh.m_vertices[indices[i]].normal += n;
		mesh.m_vertices[indices[i+1]].normal += n;
		mesh.m_vertices[indices[i+2]].normal += n;
	}

	mesh.m_texture = "pixel";

	for (size_t i = 0; i < vertices.size(); i += 3)
	{
		Vec2 v;
		v[0] = 0.2f;
		v[1] = 0.2f;
		mesh.m_vertices[i].textureCoords = v;
	}

	return std::move(mesh);
}