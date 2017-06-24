#pragma once

#include <unordered_map>

#include "predeclarations.hpp"

namespace Jo {
	namespace Files {
		class MetaFileWrapper;
	}
}

namespace Graphic {

	enum struct Effects {
		MESH,
		DEFFERED_LIGHT,
		BACKGROUNDSTAR,
//		PARTICLES,
		TEXTURE_2DQUAD,
		TEXT,
		LINES,
		SCREEN_OUTPUT,
		BLACKHOLE,
		PARTICLE_BLOB,
		PARTICLE_TEXQAUD,
		COUNT
	};

	// Predefined Uniform Buffer Objects
	enum struct UniformBuffers
	{
		GLOBAL = 0,
		CAMERA = 1,
		SIMPLE_OBJECT,
		OBJECT_LINES,
		OBJECT_GEOMETRY,
		COUNT
	};

	// Predefined sampler states
	enum struct SamplerStates
	{
		POINT = 0,
		LINEAR = 1,
		LINEAR_NOMIPMAP = 2, ///< Performs no filter on mipmaps. Useful for linearly sampling textures without mipmaps.
		COUNT			///< Number of effects - this must be the last enumeration member
	};

	/// \brief Predefined Fonts.
	enum struct Fonts
	{
		DEFAULT = 0,	///< Font used if nothing else is specified
		COUNT			///< Number of effects - this must be the last enumeration member
	};

	class Resources
	{
	public:
		static Effect& GetEffect(Effects _effect);
		static Graphic::UniformBuffer& Resources::GetUBO(UniformBuffers _ubo);
		static Graphic::SamplerState& GetSamplerState(SamplerStates _state);
		static Font& GetFont(Fonts _font);

		static Jo::Files::MetaFileWrapper& GetTextureMap();
		static Mesh& GetMesh(const std::string& _name);
		static Texture& GetTexture(const std::string& _name);

		static void Unload();
	private:
		static Effect* effects[(unsigned)Effects::COUNT];
		static Graphic::UniformBuffer* uniformBuffers[(unsigned)UniformBuffers::COUNT];
		static SamplerState* samplers[(unsigned)SamplerStates::COUNT];
		static Font* fonts[(unsigned)Fonts::COUNT];
		static Jo::Files::MetaFileWrapper* textureMap;
		static std::unordered_map<std::string, Mesh*> meshes;
		static std::unordered_map<std::string, Texture*> textures;
	};
}