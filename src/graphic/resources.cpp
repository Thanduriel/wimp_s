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
		int ind = (int)_ubo;
		if (uniformBuffers[ind]) return *uniformBuffers[ind];

		// Not loaded yet
		switch (_ubo)
		{
		case UniformBuffers::GLOBAL: {
			uniformBuffers[ind] = new UniformBuffer("Global");
			uniformBuffers[ind]->AddAttribute("Aspect", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[ind]->AddAttribute("Width", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[ind]->AddAttribute("Height", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[ind]->AddAttribute("InvWidth", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[ind]->AddAttribute("InvHeight", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[ind]->AddAttribute("Time", Graphic::UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			ei::IVec2 size = Graphic::Device::GetBackbufferSize();
			(*uniformBuffers[ind])["Aspect"] = Graphic::Device::GetAspectRatio();
			(*uniformBuffers[ind])["Width"] = (float)size[0];
			(*uniformBuffers[ind])["Height"] = (float)size[1];
			(*uniformBuffers[ind])["InvWidth"] = 1.0f / size[0];
			(*uniformBuffers[ind])["InvHeight"] = 1.0f / size[1];
			break; }
		case UniformBuffers::CAMERA:
			uniformBuffers[ind] = new UniformBuffer("Camera");
			uniformBuffers[ind]->AddAttribute("ViewProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX);
			break;

		case UniformBuffers::OBJECT_MESH:
			uniformBuffers[ind] = new UniformBuffer("Object");
			uniformBuffers[ind]->AddAttribute("c_ModelViewProjection", UniformBuffer::ATTRIBUTE_TYPE::MATRIX);
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