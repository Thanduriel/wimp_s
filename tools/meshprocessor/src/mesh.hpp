#pragma once

#include "vec.hpp"
#include <vector>
#include <assimp/Importer.hpp>

class Mesh
{
public:
	Mesh(const std::string& _pFile);
	void Save(const std::string& _name);

	// Returns the radius of the minimum sized sphere
	// positioned at point 0, that contains all vertices.
	float GetBoundingRadius() const { return m_boundingRadius; }
	const Vec3& GetLowerBound() const { return m_lowerBound; }
	const Vec3& GetUpperBound() const { return m_upperBound; }
private:
//	#pragma pack(1)
	struct Vertex
	{
		Vec3 position;
		Vec3 normal;
		Vec2 textureCoords;
	};
	std::vector<Vertex> m_vertices;
	std::vector<Vec<int, 3>> m_faces;
	std::string m_texture;

	bool ImportModel(const std::string& _pFile);
	void SceneProcessing(const aiScene* _scene);
	void ComputeBoundingValues(const aiVector3D* _vertices, size_t _numVertices);

	float m_boundingRadius;
	Vec3 m_lowerBound;
	Vec3 m_upperBound;
};
