#include "mesh.hpp"
#include "core/device.hpp"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

namespace Graphic {

	using namespace ei;

	
	Mesh::Mesh(const std::string& _pFile)
		: m_vertices(VertexArrayBuffer::PrimitiveType::TRIANGLE_LIST,
		{ {VertexAttribute::VEC3, 0}, {VertexAttribute::VEC3, 1}, {VertexAttribute::VEC2, 2} })
		,m_texture(std::vector<std::string>())
	{
		//Vertex* vertex = (Vertex*)malloc(36 * sizeof(Vertex));

		//vertex[3].position = Vec3(-1.0f, -1.0f, -1.0f);
		//vertex[4].position = Vec3(- 1.0f, -1.0f, 1.0f);
		//vertex[5].position = Vec3(- 1.0f, 1.0f, 1.0f);
		//vertex[6].position = Vec3(1.0f, 1.0f, -1.0f);
		//vertex[7].position = Vec3(-1.0f, -1.0f, -1.0f);
		//vertex[8].position = Vec3(-1.0f, 1.0f, -1.0f);
		//vertex[9].position = Vec3(1.0f, -1.0f, 1.0f);
		//vertex[10].position = Vec3(-1.0f, -1.0f, -1.0f);
		//vertex[11].position = Vec3(1.0f, -1.0f, -1.0f);
		//vertex[12].position = Vec3(1.0f, 1.0f, -1.0f);
		//vertex[13].position = Vec3(1.0f, -1.0f, -1.0f);
		//vertex[14].position = Vec3(-1.0f, -1.0f, -1.0f);
		//vertex[15].position = Vec3(-1.0f, -1.0f, -1.0f);
		//vertex[16].position = Vec3(-1.0f, 1.0f, 1.0f);
		//vertex[17].position = Vec3(-1.0f, 1.0f, -1.0f);
		//vertex[18].position = Vec3(1.0f, -1.0f, 1.0f);
		//vertex[19].position = Vec3(-1.0f, -1.0f, 1.0f);
		//vertex[20].position = Vec3(-1.0f, -1.0f, -1.0f);
		//vertex[21].position = Vec3(-1.0f, 1.0f, 1.0f);
		//vertex[22].position = Vec3(-1.0f, -1.0f, 1.0f);
		//vertex[23].position = Vec3(1.0f, -1.0f, 1.0f);
		//vertex[24].position = Vec3(1.0f, 1.0f, 1.0f);
		//vertex[25].position = Vec3(1.0f, -1.0f, -1.0f);
		//vertex[26].position = Vec3(1.0f, 1.0f, -1.0f);
		//vertex[27].position = Vec3(1.0f, -1.0f, -1.0f);
		//vertex[28].position = Vec3(1.0f, 1.0f, 1.0f);
		//vertex[29].position = Vec3(1.0f, -1.0f, 1.0f);
		//vertex[30].position = Vec3(1.0f, 1.0f, 1.0f);
		//vertex[31].position = Vec3(1.0f, 1.0f, -1.0f);
		//vertex[32].position = Vec3(-1.0f, 1.0f, -1.0f);
		//vertex[33].position = Vec3(1.0f, 1.0f, 1.0f);
		//vertex[34].position = Vec3(-1.0f, 1.0f, -1.0f);
		//vertex[35].position = Vec3(-1.0f, 1.0f, 1.0f);
		//vertex[0].position = Vec3(1.0f, 1.0f, 1.0f);
		//vertex[1].position = Vec3(-1.0f, 1.0f, 1.0f);
		//vertex[2].position = Vec3(1.0f, -1.0f, 1.0f);

		//// for now store colors
		//vertex[0].normal = Vec3(0.583f, 0.771f, 0.014f);
		//vertex[1].normal = Vec3(0.609f, 0.115f, 0.436f);
		//vertex[2].normal = Vec3(0.327f, 0.483f, 0.844f);
		//vertex[3].normal = Vec3(0.822f, 0.569f, 0.201f);
		//vertex[4].normal = Vec3(0.435f, 0.602f, 0.223f);
		//vertex[5].normal = Vec3(0.310f, 0.747f, 0.185f);
		//vertex[6].normal = Vec3(0.597f, 0.770f, 0.761f);
		//vertex[7].normal = Vec3(0.559f, 0.436f, 0.730f);
		//vertex[8].normal = Vec3(0.359f, 0.583f, 0.152f);
		//vertex[9].normal = Vec3(0.483f, 0.596f, 0.789f);
		//vertex[10].normal = Vec3(0.559f, 0.861f, 0.639f);
		//vertex[11].normal = Vec3(0.195f, 0.548f, 0.859f);
		//vertex[12].normal = Vec3(0.014f, 0.184f, 0.576f);
		//vertex[13].normal = Vec3(0.771f, 0.328f, 0.970f);
		//vertex[14].normal = Vec3(0.406f, 0.615f, 0.116f);
		//vertex[15].normal = Vec3(0.676f, 0.977f, 0.133f);
		//vertex[16].normal = Vec3(0.971f, 0.572f, 0.833f);
		//vertex[17].normal = Vec3(0.140f, 0.616f, 0.489f);
		//vertex[18].normal = Vec3(0.997f, 0.513f, 0.064f);
		//vertex[19].normal = Vec3(0.945f, 0.719f, 0.592f);
		//vertex[20].normal = Vec3(0.543f, 0.021f, 0.978f);
		//vertex[21].normal = Vec3(0.279f, 0.317f, 0.505f);
		//vertex[22].normal = Vec3(0.167f, 0.620f, 0.077f);
		//vertex[23].normal = Vec3(0.347f, 0.857f, 0.137f);
		//vertex[24].normal = Vec3(0.055f, 0.953f, 0.042f);
		//vertex[25].normal = Vec3(0.714f, 0.505f, 0.345f);
		//vertex[26].normal = Vec3(0.783f, 0.290f, 0.734f);
		//vertex[27].normal = Vec3(0.722f, 0.645f, 0.174f);
		//vertex[28].normal = Vec3(0.302f, 0.455f, 0.848f);
		//vertex[29].normal = Vec3(0.225f, 0.587f, 0.040f);
		//vertex[30].normal = Vec3(0.517f, 0.713f, 0.338f);
		//vertex[31].normal = Vec3(0.053f, 0.959f, 0.120f);
		//vertex[32].normal = Vec3(0.393f, 0.621f, 0.362f);
		//vertex[33].normal = Vec3(0.673f, 0.211f, 0.457f);
		//vertex[34].normal = Vec3(0.820f, 0.883f, 0.371f);
		//vertex[35].normal = Vec3(0.982f, 0.099f, 0.879);

		//m_vertices.GetBuffer(0)->SetData((void*&)vertex, 36 * sizeof(Vertex));
		ImportModel(_pFile);
	/*	Vertex* vertex = (Vertex*)malloc(3 * sizeof(Vertex));
		vertex[1].position = Vec3(-15.0f, 0.0f, -15.0f);
		vertex[0].position = Vec3(15.0f, 0.0f, -15.0f);
		vertex[2].position = Vec3(0.0f, 0.0f, 15.0f);
		vertex[0].normal = Vec3(0.f, 1.f, 0.f);
		vertex[1].normal = Vec3(0.f, 1.f, 0.f);
		vertex[2].normal = Vec3(0.f, 1.f, 0.f);

		m_vertices.GetBuffer(0)->SetData((void*&)vertex, 3 * sizeof(Vertex));*/


	}

	bool Mesh::ImportModel(const std::string& _pFile)
	{
		// Create an instance of the Importer class
		Assimp::Importer importer;
		// And have it read the given file with some example postprocessing
		// Usually - if speed is not the most important aspect for you - you'll 
		// propably to request more postprocessing than we do in this example.
		const aiScene* scene = importer.ReadFile(_pFile,
			aiProcess_Triangulate|
			aiProcess_GenUVCoords|
			aiProcess_GenNormals|
			aiProcess_FixInfacingNormals);

		// If the import failed, report it
		if (!scene)
		{	
			// TODO: implement error logging 
			//DoTheErrorLogging(importer.GetErrorString());
			return false;
		}
		// Now we can access the file's contents. 
		SceneProcessing(scene);
		// We're done. Everything will be cleaned up by the importer destructor
		return true;
	}

	template<typename T, unsigned N, typename A>
	ei::Matrix<T, N, 1>& assign(ei::Matrix<T,N,1>& _lhs, const A& _rhs)
	{
		for (unsigned i = 0; i < N; ++i)
			_lhs[i] = _rhs[i];

		return _lhs;
	}

	void Mesh::SceneProcessing(const aiScene* _scene) 
	{
		unsigned int totalVertexNum = 0;
		int totalMaterialNum = 0;
		Vec3 vertex;
		Vec3 normal;
		Vec2 textureC;
		Vertex* v;
		
		for (unsigned int i = 0; i < _scene->mNumMeshes; i++) // count all vertices in all meshes
		{
			aiMesh *mesh = _scene->mMeshes[i];
			totalVertexNum += mesh->mNumVertices;
			
			if (mesh->mMaterialIndex >= 0) 
			{
				totalMaterialNum++;
			}
		}
		
		aiMesh *mesh = _scene->mMeshes[0];
		v = (Vertex*)malloc(mesh->mNumFaces * 3 * sizeof(Vertex)); //allocate memory of total number of vertices
		
		for (unsigned int j = 0; j < mesh->mNumFaces; j++) // process vertex positions, normals and texture coordinates
		{
			Assert(mesh->mFaces[j].mNumIndices == 3, "Primitives should only be triangles.");
			unsigned ind = j * 3;
			assign(vertex, mesh->mVertices[mesh->mFaces[j].mIndices[0]]);
			v[ind].position = vertex;
			assign(vertex, mesh->mVertices[mesh->mFaces[j].mIndices[1]]);
			v[ind +1].position = vertex;
			assign(vertex, mesh->mVertices[mesh->mFaces[j].mIndices[2]]);
			v[ind +2].position = vertex;

			assign(vertex, mesh->mNormals[mesh->mFaces[j].mIndices[0]]);
			v[ind].normal = vertex;
			assign(vertex, mesh->mNormals[mesh->mFaces[j].mIndices[1]]);
			v[ind+1].normal = vertex;
			assign(vertex, mesh->mNormals[mesh->mFaces[j].mIndices[2]]);
			v[ind+2].normal = vertex;

			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				vertex.x = mesh->mTextureCoords[0][j].x;
				vertex.y = mesh->mTextureCoords[0][j].y;
				v[j].textureCoords = textureC;
			}
			else
				v[j].textureCoords = Vec2(0.0f, 0.0f);


			// Texture loading (Diffuse and Specular maps)
			if (mesh->mMaterialIndex >= 0)
			{
				aiMaterial *material = _scene->mMaterials[mesh->mMaterialIndex];
				std::vector<std::string> textures;
				for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
				{
					aiString str;
					material->GetTexture(aiTextureType_DIFFUSE, i, &str);
					textures[i] = str.C_Str();
					textures.insert(textures.end(),(std::string)"|");
				}

				for (unsigned int i = textures.size(); i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
				{
					aiString str;
					material->GetTexture(aiTextureType_SPECULAR, i, &str);
					textures[i] = str.C_Str();
				}
				m_texture = Graphic::Texture(textures); //till the "|" seperator all textures are Diffuse maps, after that Specular maps
			}
		}
		
		m_vertices.GetBuffer(0)->SetData((void*&)v, mesh->mNumFaces * 3 * sizeof(Vertex)); // set m_vertices with all vertex data (position,normal,texturecoords)
	}

	void Mesh::Draw()
	{
		Device::DrawVertices(m_vertices, 0, m_vertices.GetNumVertices());
		
	}
}