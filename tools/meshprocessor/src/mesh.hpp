#pragma once

#include "vec.hpp"
#include <vector>
#include <assimp/Importer.hpp>

class Mesh
{
public:
	enum struct Format {
		Indexed,
		Flat
	};
	const static uint16_t FORMAT_VERSION = 2;

	Mesh() = default;
	Mesh(const std::string& _pFile);
	void Save(const std::string& _name, Format _format = Format::Flat);

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
	std::vector<Vec3> m_normals;
	std::string m_texture;

	bool ImportModel(const std::string& _pFile);
	void SceneProcessing(const aiScene* _scene);
	void ComputeBoundingValues();
	// Vertex buffer without indices
	std::vector<Vertex> Flatten();

	float m_boundingRadius;
	Vec3 m_lowerBound;
	Vec3 m_upperBound;

	friend class ConvexHullGen;
};
