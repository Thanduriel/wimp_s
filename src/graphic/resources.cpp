#include "resources.hpp"
#include "core/effect.hpp"
#include "utils/assert.hpp"
#include "core/uniformbuffer.hpp"
#include "core/device.hpp"

namespace Graphic {

	Graphic::Effect* Resources::effects[];
	Graphic::UniformBuffer* Resources::uniformBuffers[];

	// ****************************************************** //
	Effect& Resources::GetEffect(Effects _effect)
	{
		if (effects[(int)_effect]) return *effects[(int)_effect];

		// load new shader
		switch (_effect)
		{
		case Effects::MESH:
			effects[(int)_effect] = new Effect("shader/mesh.vs", "shader/mesh.ps");
			effects[(int)_effect]->SetBlendState(BlendState(BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE));
			effects[(int)_effect]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
		//	effects[(int)_effect]->BindUniformBuffer(GetUbo(UniformBuffers::GLOBAL));
			break;

		default:
			Assert(false, "This effect is not implemented.");
			break;
		}

		return *effects[(int)_effect];
	}

	// ****************************************************** //
	Graphic::UniformBuffer& Resources::GetUBO(UniformBuffers _ubo)
	{
		if (uniformBuffers[(int)_ubo]) return *uniformBuffers[(int)_ubo];

		// Not loaded yet
		switch (_ubo)
		{
		case UniformBuffers::GLOBAL: {
			uniformBuffers[(int)_ubo] = new UniformBuffer("Global");
			uniformBuffers[(int)_ubo]->AddAttribute("Aspect", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[(int)_ubo]->AddAttribute("Width", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[(int)_ubo]->AddAttribute("Height", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[(int)_ubo]->AddAttribute("InvWidth", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[(int)_ubo]->AddAttribute("InvHeight", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[(int)_ubo]->AddAttribute("Time", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			ei::IVec2 size = Graphic::Device::GetBackbufferSize();
			(*uniformBuffers[(int)_ubo])["Aspect"] = Graphic::Device::GetAspectRatio();
			(*uniformBuffers[(int)_ubo])["Width"] = (float)size[0];
			(*uniformBuffers[(int)_ubo])["Height"] = (float)size[1];
			(*uniformBuffers[(int)_ubo])["InvWidth"] = 1.0f / size[0];
			(*uniformBuffers[(int)_ubo])["InvHeight"] = 1.0f / size[1];
			break; }
		case UniformBuffers::CAMERA:
			uniformBuffers[(int)_ubo] = new UniformBuffer("Camera");
			uniformBuffers[(int)_ubo]->AddAttribute("ViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX);
			break;
		default:
			Assert(false, "This uniform buffer is not implemented.");
			break;
		}

		return *uniformBuffers[(int)_ubo];
	}

	// ****************************************************** //
	void Resources::Unload()
	{
		for (int i = 0; i < (int)Effects::COUNT; ++i)
			delete effects[i];
	}
}