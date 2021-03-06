#pragma once

#include <unordered_map>
#include "utils/stdextensions.hpp"

#include "predeclarations.hpp"

#ifdef NDEBUG
#define RESOURCE_PATH ""
#else
#define RESOURCE_PATH "../"
#endif

namespace Jo {
	namespace Files {
		class MetaFileWrapper;
	}
}

namespace Graphic {

	enum struct Effects {
		MESH,
		SHIELD,
		DEFFERED_LIGHT,
		DARKEN,
		DD_LIGHT,
		BACKGROUNDSTAR,
		TEXTURE_2DQUAD,
		TEXT,
		LINES,
		SCREEN_OUTPUT,
		BLACKHOLE,
		SUN,
		PARTICLE_BLOB,
		PARTICLE_RAY,
		PARTICLE_TEXQAUD,
		BRIGHTNESS,
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
		OBJECT_SHIELD,
		DOMINANT_LIGHT,
		BRIGHTNESS_PARAMS,
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
		static Graphic::UniformBuffer& GetUBO(UniformBuffers _ubo);
		static Graphic::SamplerState& GetSamplerState(SamplerStates _state);
		static Font& GetFont(Fonts _font);

		static Jo::Files::MetaFileWrapper& GetTextureMap();
		// the texture can be overwritten
		static Mesh& GetMesh(const std::string& _name, const std::string& _textureName = "");
		static RawMesh& GetRawMesh(const std::string& _name);
		static Texture& GetTexture(const std::string& _name);

		static void Unload();
	private:
		static Effect* effects[(unsigned)Effects::COUNT];
		static Graphic::UniformBuffer* uniformBuffers[(unsigned)UniformBuffers::COUNT];
		static SamplerState* samplers[(unsigned)SamplerStates::COUNT];
		static Font* fonts[(unsigned)Fonts::COUNT];
		static Jo::Files::MetaFileWrapper* textureMap;
		static std::unordered_map<std::pair<std::string, std::string>, Mesh*> meshes;
		static std::unordered_map<std::string, RawMesh*> rawMeshes;
		static std::unordered_map<std::string, Texture*> textures;
	};
}