
#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>

#include "mesh.hpp"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "pathutils.hpp"

//#define COMPACT_FORMAT

const std::unordered_map<std::string, Mesh::Format> Mesh::FORMAT_NAMES =
{
	{"Indexed", Mesh::Format::Indexed},
	{ "IndexedNormal", Mesh::Format::IndexedNormal },
	{ "Flat", Mesh::Format::Flat }
};


	Mesh::Mesh(const std::string& _pFile)
	{
		ImportModel(_pFile);
	}

	template<typename T>
	void write(std::ofstream& _stream, const T& _data)
	{
		_stream.write(reinterpret_cast<const char*>(&_data), sizeof(T));
	}

	void write(std::ofstream& _stream, const std::string& _str)
	{
		_stream << _str;
		_stream << '\0';
	}

	void Mesh::Save(const std::string& _name, Format _format, bool _flatNormals, const std::string& _textureName)
	{
		Mesh::ComputeBoundingValues();

		std::string name = _name + (_format == Format::Flat ? ".wim" : ".wii");
		std::ofstream file(name, std::ios::binary);

		uint16_t version = FORMAT_VERSION;
		write(file, version);
		uint16_t format = static_cast<uint16_t>(_format);
		write(file, format);
		// texture name
		if (_textureName != "") write(file, _textureName);
		else write(file, m_texture);

		// computed parameters
		write(file, m_boundingRadius);
		write(file, m_lowerBound);
		write(file, m_upperBound);

		if (_format == Format::Indexed)
		{
			// number of vertices
			uint32_t s = static_cast<uint32_t>(m_vertices.size());
			write(file, s);

			write(file, m_faces.size());
			// data
			file.write(reinterpret_cast<char*>(&m_vertices.front()), m_vertices.size() * sizeof(Vertex));
			file.write(reinterpret_cast<char*>(&m_faces.front()), m_faces.size() * sizeof(Vec<int, 3>));
		}
		else if (_format == Format::IndexedNormal)
		{
		//	assert(m_faces.size() == m_normals.size() && "Expecting one normal per face.");
			uint32_t s = static_cast<uint32_t>(m_vertices.size());
			write(file, s);
			s = static_cast<uint32_t>(m_faces.size());
			write(file, s);

			for (auto& v : m_vertices)
				file.write(reinterpret_cast<char*>(&v.position), sizeof(Vec3));

			file.write(reinterpret_cast<char*>(&m_faces.front()), m_faces.size() * sizeof(Vec<int, 3>));
		//	file.write(reinterpret_cast<char*>(&m_normals.front()), m_normals.size() * sizeof(Vec3));
		}
		else if (_format == Format::Flat)
		{
			auto vertices = Flatten(_flatNormals);
			uint32_t s = static_cast<uint32_t>(vertices.size());
			write(file, s);

			file.write(reinterpret_cast<char*>(&vertices.front()), vertices.size() * sizeof(Vertex));
		}

		// sockets
		uint32_t s = static_cast<uint32_t>(m_sockets.size());
		write(file, s);
		for (auto& socket : m_sockets)
		{
			write(file, socket.name);
			write(file, socket.position);
		}
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
			aiProcess_ImproveCacheLocality | // could improve performance
			aiProcess_JoinIdenticalVertices |
			aiProcess_FixInfacingNormals);

		if (!scene)
			throw std::string("Failed to load mesh: ") + importer.GetErrorString();

		SceneProcessing(scene);
		return true;
	}

	void Mesh::SceneProcessing(const aiScene* _scene) 
	{	
		aiMesh *mesh = _scene->mMeshes[0];
		
		m_vertices.resize(mesh->mNumVertices);
		m_normals.resize(mesh->mNumFaces);
		Vertex* v = &m_vertices[0];

		for (unsigned int j = 0; j < mesh->mNumFaces; j++) // process vertex positions, normals and texture coordinates
		{
			auto indices = mesh->mFaces[j].mIndices;
			m_faces.emplace_back(indices[0], indices[1], indices[2]);
			m_normals[j] = (mesh->mNormals[indices[0]] + mesh->mNormals[indices[1]] + mesh->mNormals[indices[2]]).Normalize();
		}
		for (unsigned i = 0; i < mesh->mNumVertices; ++i)
		{
			v[i].position = mesh->mVertices[i];
			v[i].normal = mesh->mNormals[i];

			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				v[i].textureCoords = mesh->mTextureCoords[0][i];
			}
		}

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

		// sockets encoded as lights
		for (unsigned i = 0; i < _scene->mNumLights; ++i)
		{
			Socket s;
			s.name = _scene->mLights[i]->mName.C_Str();
			auto node = _scene->mRootNode->FindNode(s.name.c_str());
			aiQuaternion rot;
			aiVector3D pos;
			node->mTransformation.DecomposeNoScaling(rot, pos);
			s.position = pos * 1.f/100.f;
			// somehow this matrix is rotated when imported
			// this should invert the rotation
			float tmp = s.position[1];
			s.position[1] = -s.position[2];
			s.position[2] = tmp;
			m_sockets.push_back(s);
			std::clog << "found socket: " << s.name << " with position (" << s.position[0] << "; " << s.position[1] << "; " << s.position[2] << ")" << std::endl;
		}
	}

	void Mesh::FlipNormals()
	{
		for (auto& n : m_normals) n = n * -1.f;
	}

	// ****************************************************************** //
	void Mesh::ComputeBoundingValues()
	{
		float maxDist = 0.f;

		for (size_t i = 0; i < m_vertices.size(); ++i)
		{
			float val = m_vertices[i].position[0] * m_vertices[i].position[0]
				+ m_vertices[i].position[1] * m_vertices[i].position[1]
				+ m_vertices[i].position[2] * m_vertices[i].position[2];
			if (val > maxDist) maxDist = val;
		}

		m_boundingRadius = sqrt(maxDist);

		auto xval = std::minmax_element(m_vertices.begin(), m_vertices.end(), [](const Vertex& _lhs, const Vertex& _rhs)
		{
			return _lhs.position[0] < _rhs.position[0];
		});
		auto yval = std::minmax_element(m_vertices.begin(), m_vertices.end(), [](const Vertex& _lhs, const Vertex& _rhs)
		{
			return _lhs.position[1] < _rhs.position[1];
		});
		auto zval = std::minmax_element(m_vertices.begin(), m_vertices.end(), [](const Vertex& _lhs, const Vertex& _rhs)
		{
			return _lhs.position[2] < _rhs.position[2];
		});

		m_lowerBound = Vec3(xval.first->position[0], yval.first->position[1], zval.first->position[2]);
		m_upperBound = Vec3(xval.second->position[0], yval.second->position[1], zval.second->position[2]);
	}

	std::vector<Mesh::Vertex> Mesh::Flatten(bool _normals)
	{
		std::vector<Vertex> vertices;
		vertices.reserve(m_faces.size() * 3);

		for (size_t i = 0; i < m_faces.size(); ++i)
		{
			Vec3 n{};
			vertices.emplace_back(m_vertices[m_faces[i][0]]);
			n += vertices.back().normal;
			vertices.emplace_back(m_vertices[m_faces[i][1]]);
			n += vertices.back().normal;
			vertices.emplace_back(m_vertices[m_faces[i][2]]);
			n += vertices.back().normal;

			float l = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
			n[0] /= l; n[1] /= l; n[2] /= l;
			auto it = vertices.end();
			(--it)->normal = n;
			(--it)->normal = n;
			(--it)->normal = n;
		}

		return std::move(vertices);
	}