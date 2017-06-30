#pragma once

#include "core/texture.hpp"
#include "core/vertexbuffer.hpp"
#include <assimp/Importer.hpp>

namespace Graphic {

	class Mesh
	{
	public:
		Mesh();
		Mesh(const std::string& _pFile);

		void Draw() const;
	private:
		struct Vertex
		{
			ei::Vec3 position;
			ei::Vec3 normal;
			ei::Vec2 textureCoords;
		};
		Texture* m_texture;
		VertexArrayBuffer m_vertices;
		bool ImportModel(const std::string& _pFile);
		void SceneProcessing(const aiScene* _scene);
	};
}