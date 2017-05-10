#pragma once

#include "predeclarations.hpp"

namespace Graphic {

	enum struct Effects {
		MESH,
//		DEFFERED_LIGHT,
//		STARBACKGROUND,
//		PARTICLES,
		COUNT
	};

	enum struct UniformBuffers
	{
		GLOBAL = 0,
		CAMERA = 1,
		OBJECT_MESH,
		COUNT
	};

	class Resources
	{
	public:
		static Effect& GetEffect(Effects _effect);
		static Graphic::UniformBuffer& Resources::GetUBO(UniformBuffers _ubo);

		static void Unload();
	private:
		static Effect* effects[(unsigned)Effects::COUNT];
		static Graphic::UniformBuffer* uniformBuffers[(unsigned)UniformBuffers::COUNT];
	};
}