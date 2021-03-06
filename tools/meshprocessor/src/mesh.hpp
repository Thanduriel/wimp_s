#pragma once

#include "vec.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <assimp/Importer.hpp>

class Mesh
{
public:
	enum struct Format {
		Indexed,
		IndexedNormal,
		Flat,
		COUNT
	};
	const static std::unordered_map<std::string, Format> FORMAT_NAMES;

	const static uint16_t FORMAT_VERSION = 4;

	Mesh() = default;
	Mesh(const std::string& _pFile);
	void Save(const std::string& _name, Format _format = Format::Flat, bool _flatNormals = false, const std::string& _textureName = "");
	void FlipNormals();

	void Scale(float _scale);
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

	struct Socket
	{
		std::string name;
		Vec3 position;
	};

	std::vector< Socket > m_sockets;

	bool ImportModel(const std::string& _pFile);
	void SceneProcessing(const aiScene* _scene);
	void ComputeBoundingValues();
	// Vertex buffer without indices
	std::vector<Vertex> Flatten(bool _normals);

	float m_boundingRadius;
	Vec3 m_lowerBound;
	Vec3 m_upperBound;

	friend class ConvexHullGen;
};
