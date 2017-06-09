#include "resources.hpp"
#include "core/effect.hpp"
#include "utils/assert.hpp"
#include "core/uniformbuffer.hpp"
#include "core/device.hpp"
#include "core/texture.hpp"
#include "graphic/interface/font.hpp"
#include "jofilelib.hpp"

namespace Graphic {

	Effect* Resources::effects[];
	UniformBuffer* Resources::uniformBuffers[];
	SamplerState* Resources::samplers[];
	Font* Resources::fonts[];
	Jo::Files::MetaFileWrapper* Resources::textureMap;

	// ****************************************************** //
	Effect& Resources::GetEffect(Effects _effect)
	{
		int ind = (int)_effect;
		if (effects[ind]) return *effects[ind];

		// load new shader
		switch (_effect)
		{
		case Effects::MESH:
			effects[ind] = new Effect("shader/mesh.vs", "shader/mesh.ps");
			effects[ind]->SetBlendState(BlendState(BlendState::BLEND_OPERATION::DISABLE, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::LESS, true));
			// the example cube has all faces inverted
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::SIMPLE_OBJECT));
			break;
		case Effects::DEFFERED_LIGHT:
			effects[ind] = new Effect("shader/light.vs", "shader/light.ps", "shader/light.gs");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::ADD, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->BindTexture("screenTex", 0, Resources::GetSamplerState(SamplerStates::POINT));
			effects[ind]->BindTexture("depthTex", 1, Resources::GetSamplerState(SamplerStates::POINT));
		//	effects[ind]->BindTexture("normalTex", 2, Resources::GetSamplerState(SamplerStates::POINT));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::SIMPLE_OBJECT));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::GLOBAL));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::CAMERA));
			break;
		case Effects::TEXTURE_2DQUAD:
			effects[ind] = new Effect("shader/screentex.vs", "shader/screentex.ps", "shader/screentex.gs");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::ADD, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::INV_SRC_ALPHA));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->BindTexture("u_screenTex", 7, GetSamplerState(SamplerStates::LINEAR));
			break;
		case Effects::BACKGROUNDSTAR:
			effects[ind] = new Effect("shader/backgroundstar.vs", "shader/backgroundstar.ps", "shader/backgroundstar.gs");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::ADD, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::CAMERA));
			break;
		case Effects::LINES:
			effects[ind] = new Effect("shader/lines.vs", "shader/lines.ps", "shader/lines.gs");
			effects[ind]->SetBlendState(BlendState(BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::LESS, false));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::OBJECT_LINES));
			break;
		case Effects::SCREEN_OUTPUT:
			effects[ind] = new Effect("shader/fullscreentri.vs", "shader/output.ps");
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->SetDepthStencilState(DepthStencilState(DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->BindUniformBuffer(Resources::GetUBO(UniformBuffers::GLOBAL));
			effects[ind]->BindTexture("screenTex", 0, Resources::GetSamplerState(SamplerStates::POINT));
			break;
		case Effects::BLACKHOLE:
			effects[ind] = new Effect("shader/postprocessing/blackhole.vs", "shader/postprocessing/blackhole.ps", "shader/postprocessing/blackhole.gs");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::DISABLE, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::SIMPLE_OBJECT));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::GLOBAL));
			effects[ind]->BindTexture("screenTex", 0, Resources::GetSamplerState(SamplerStates::LINEAR));
			break;
		default:
			Assert(false, "This effect is not implemented.");
			break;
		}

		return *effects[ind];
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
			// The projection matrix is very sparse. That can be used
			// explicitly to reduce constant buffer size and arithmetic
			// instructions in shader:
			// x 0 0 0
			// 0 y 0 0
			// 0 0 z 1
			// 0 0 w 0
			// The projection vector contains (x, y, z, w)
			// Usage: vec4(pos.xyz * proj.xyz + vec3(0,0,proj.w), pos.z)
			uniformBuffers[ind]->AddAttribute("Projection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4);
			// The inverse projection vector contains (1/x, 1/y, 1/z, -w/z)
			// Usage: pos.xyz * invProj.xyz + vec3(0,0,invProj.w)
			uniformBuffers[ind]->AddAttribute("InverseProjection", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4);
			// zw are respective half size
			uniformBuffers[ind]->AddAttribute("NearPlaneSize", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC4);
			uniformBuffers[ind]->AddAttribute("CameraRotation", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX);
			break;

		case UniformBuffers::SIMPLE_OBJECT:
			uniformBuffers[ind] = new UniformBuffer("Object");
			uniformBuffers[ind]->AddAttribute("c_WorldViewProjection", UniformBuffer::ATTRIBUTE_TYPE::MATRIX);
			break;
		case UniformBuffers::OBJECT_LINES:
			uniformBuffers[ind] = new UniformBuffer("Object");
			uniformBuffers[ind]->AddAttribute("c_WorldViewProjection", UniformBuffer::ATTRIBUTE_TYPE::MATRIX);
			uniformBuffers[ind]->AddAttribute("c_Color", UniformBuffer::ATTRIBUTE_TYPE::VEC4);
			uniformBuffers[ind]->AddAttribute("c_Thickness", UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			break;
		default:
			Assert(false, "This uniform buffer is not implemented.");
			break;
		}

		return *uniformBuffers[(int)_ubo];
	}

	// ********************************************************************* //
	Graphic::SamplerState& Resources::GetSamplerState(SamplerStates _state)
	{
		int ind = (int)_state;
		if (samplers[ind]) return *samplers[ind];

		// Not loaded yet
		switch (_state)
		{
		case SamplerStates::POINT:
			samplers[ind] = new SamplerState(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::POINT,
				Graphic::SamplerState::SAMPLE::POINT, Graphic::SamplerState::SAMPLE::POINT);
			break;
		case SamplerStates::LINEAR_NOMIPMAP:
			samplers[ind] = new SamplerState(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::LINEAR,
				Graphic::SamplerState::SAMPLE::LINEAR, Graphic::SamplerState::SAMPLE::POINT);
			break;
		case SamplerStates::LINEAR:
			samplers[ind] = new SamplerState(Graphic::SamplerState::EDGE_TREATMENT::WRAP, Graphic::SamplerState::SAMPLE::LINEAR,
				Graphic::SamplerState::SAMPLE::LINEAR, Graphic::SamplerState::SAMPLE::LINEAR);
			break;
		}

		return *samplers[(int)_state];
	}

	// ****************************************************** //
	Graphic::Font& Resources::GetFont(Fonts _font)
	{
		if (fonts[(int)_font]) return *fonts[(int)_font];

		// Not loaded yet
		switch (_font)
		{
		case Fonts::DEFAULT:
			fonts[(int)_font] = new Graphic::Font("Iosevka_Medium");
			break;
		}

		return *fonts[(int)_font];
	}

	// ****************************************************** //
	Jo::Files::MetaFileWrapper& Resources::GetTextureMap()
	{
		if (!textureMap)
		{
			//test screen tex
			Jo::Files::HDDFile file("texture/combined.sraw");
			textureMap = new Jo::Files::MetaFileWrapper(file, Jo::Files::Format::SRAW);
		}

		return *textureMap;
	}

	// ****************************************************** //
	void Resources::Unload()
	{
		for (int i = 0; i < (int)Effects::COUNT; ++i)
			delete effects[i];

		for (int i = 0; i < (int)UniformBuffers::COUNT; ++i)
			if(uniformBuffers[i]) delete uniformBuffers[i];

		for (int i = 0; i < (int)SamplerStates::COUNT; ++i)
			if (samplers[i]) delete samplers[i];

		for (int i = 0; i < (int)Fonts::COUNT; ++i)
			if (fonts[i]) delete fonts[i];

		if (textureMap) delete textureMap;
	}
}