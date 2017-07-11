
#include <iostream>
#include <vector>
#include <fstream>

#include "mesh.hpp"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "pathutils.hpp"

//#define COMPACT_FORMAT

	Mesh::Mesh(const std::string& _pFile)
	{
		ImportModel(_pFile);
	}

	template<typename T>
	void write(std::ofstream& _stream, const T& _data)
	{
		_stream.write(reinterpret_cast<const char*>(&_data), sizeof(T));
	}

	void Mesh::Save(const std::string& _name)
	{
		std::string name = _name + ".wim";
		std::ofstream file(name, std::ios::binary);

		uint16_t version = 1;
		write(file, version);
		// texture name
		file << m_texture;
		file << ' ';

		// computed parameters
		write(file, m_boundingRadius);
		write(file, m_lowerBound);
		write(file, m_upperBound);

		// number of vertices
		uint32_t s = static_cast<uint32_t>(m_vertices.size());
		file.write(reinterpret_cast<char*>(&s), 4);
#ifdef COMPACT_FORMAT
		file << static_cast<uint32_t>(m_faces.size());
#endif
		// data
		size_t test = m_vertices.size() * sizeof(Vertex);
		file.write(reinterpret_cast<char*>(&m_vertices.front()), m_vertices.size() * sizeof(Vertex));
#ifdef COMPACT_FORMAT
		file.write(reinterpret_cast<char*>(&m_faces.front()), m_faces.size() * sizeof(Vec<int,3>));
#endif
	}

	bool Mesh::ImportModel(const std::string& _pFile)
	{
		// Create an instance of the Importer class
		Assimp::Importer importer;
		// And have it read the given file with some example postprocessing
		// Usually - if speed is not the most important aspect for you - you'll 
		// propably to request more postprocessing than we do in this example.
		const aiScene* scene = importer.ReadFile(_pFile,
			aiProcess_Triangulate |
			aiProcess_GenUVCoords |
			aiProcess_FlipUVs | // fbx exported by blender seem to require this
			aiProcess_GenNormals |
		aiProcess_FixInfacingNormals);

		if (!scene)
			throw std::string("Failed to load mesh: ") + importer.GetErrorString();

		SceneProcessing(scene);
		return true;
	}

	template<typename T, unsigned N, typename A>
	Vec<T,N>& assign(Vec<T,N>& _lhs, const A& _rhs)
	{
		for (unsigned i = 0; i < N; ++i)
			_lhs[i] = _rhs[i];

		return _lhs;
	}

	void Mesh::SceneProcessing(const aiScene* _scene) 
	{	
		aiMesh *mesh = _scene->mMeshes[0];
		
#ifdef COMPACT_FORMAT
		m_vertices.resize(mesh->mNumVertices);
		Vertex* v = &m_vertices[0];

		for (unsigned int j = 0; j < mesh->mNumFaces; j++) // process vertex positions, normals and texture coordinates
		{
			auto indices = mesh->mFaces[j].mIndices;
			m_faces.emplace_back(indices[0], indices[1], indices[2]);
		}
		for (unsigned i = 0; i < mesh->mNumVertices; ++i)
		{
			assign(v[i].position, mesh->mVertices[i]);

			assign(v[i].normal, mesh->mNormals[i]);

			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				assign(v[i].textureCoords, mesh->mTextureCoords[0][i]);
			}
		}
#else
		m_vertices.resize(mesh->mNumFaces * 3);
		Vertex* v = &m_vertices[0];

		for (unsigned int j = 0; j < mesh->mNumFaces; ++j) // process vertex positions, normals and texture coordinates
		{
			unsigned ind = j * 3;
			auto indices = mesh->mFaces[j].mIndices;

			assign(v[ind].position, mesh->mVertices[indices[0]]);
			assign(v[ind + 1].position, mesh->mVertices[indices[1]]);
			assign(v[ind + 2].position, mesh->mVertices[indices[2]]);

			assign(v[ind].normal, mesh->mNormals[indices[0]]);
			assign(v[ind + 1].normal, mesh->mNormals[indices[1]]);
			assign(v[ind + 2].normal, mesh->mNormals[indices[2]]);

			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				assign(v[ind + 0].textureCoords, mesh->mTextureCoords[0][indices[0]]);
				assign(v[ind + 1].textureCoords, mesh->mTextureCoords[0][indices[1]]);
				assign(v[ind + 2].textureCoords, mesh->mTextureCoords[0][indices[2]]);
			}
		}
#endif

		// Texture loading (Diffuse and Specular maps)
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = _scene->mMaterials[mesh->mMaterialIndex];
			std::vector<std::string> textures;
			for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
			{
				aiString str;
				material->GetTexture(aiTextureType_DIFFUSE, i, &str);
				textures.emplace_back(str.C_Str());
				//	textures.insert(textures.end(),(std::string)"|");
			}

			for (size_t i = textures.size(); i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
			{
				aiString str;
				material->GetTexture(aiTextureType_SPECULAR, (unsigned)i, &str);
				textures[i] = str.C_Str();
			}
			if (textures.size())
			{

				std::string name = PathUtils::GetName(PathUtils::CanonicalizePath(textures[0]));
				m_texture = name;
			}
		}
	

		// compute size properties
		ComputeBoundingValues(mesh->mVertices, mesh->mNumVertices);
	}

	void Mesh::ComputeBoundingValues(const aiVector3D* _vertices, size_t _numVertices)
	{
		float maxDist = 0.f;

		for (size_t i = 0; i < _numVertices; ++i)
		{
			float val = _vertices[i].SquareLength();
			if (val > maxDist) maxDist = val;
		}

		m_boundingRadius = sqrt(maxDist);

		auto xval = std::minmax_element(_vertices, _vertices + _numVertices, [](const aiVector3D& _lhs, const aiVector3D& _rhs)
		{
			return _lhs.x < _rhs.x;
		});
		auto yval = std::minmax_element(_vertices, _vertices + _numVertices, [](const aiVector3D& _lhs, const aiVector3D& _rhs)
		{
			return _lhs.y < _rhs.y;
		});
		auto zval = std::minmax_element(_vertices, _vertices + _numVertices, [](const aiVector3D& _lhs, const aiVector3D& _rhs)
		{
			return _lhs.z < _rhs.z;
		});

		m_lowerBound = Vec3((*xval.first)[0], (*yval.first)[1], (*zval.first)[2]);
		m_upperBound = Vec3((*xval.second)[0], (*yval.second)[1], (*zval.second)[2]);
	}