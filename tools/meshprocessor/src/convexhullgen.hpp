#pragma once
#include "mesh.hpp"

class ConvexHullGen
{
public:
	Mesh Generate(Mesh& _mesh, float _epsilon, bool _ccw);
private:
	void Set(Mesh& _mesh);

	Mesh* m_mesh;
	std::vector<Vec3> m_points;
};