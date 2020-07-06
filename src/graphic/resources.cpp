#include "resources.hpp"
#include "core/effect.hpp"
#include "utils/assert.hpp"
#include "core/uniformbuffer.hpp"
#include "core/device.hpp"
#include "core/texture.hpp"
#include "graphic/interface/font.hpp"
#include "jofilelib.hpp"
#include "mesh.hpp"

namespace Graphic {

	Effect* Resources::effects[];
	UniformBuffer* Resources::uniformBuffers[];
	SamplerState* Resources::samplers[];
	Font* Resources::fonts[];
	Jo::Files::MetaFileWrapper* Resources::textureMap;
	std::unordered_map<std::pair<std::string, std::string>, Mesh*> Resources::meshes;
	std::unordered_map<std::string, RawMesh*> Resources::rawMeshes;
	std::unordered_map<std::string, Texture*> Resources::textures;


	// ****************************************************** //
	Effect& Resources::GetEffect(Effects _effect)
	{
		int ind = (int)_effect;
		if (effects[ind]) return *effects[ind];

		// load new shader
		switch (_effect)
		{
		case Effects::MESH:
			effects[ind] = new Effect(RESOURCE_PATH "shader/mesh.vs", RESOURCE_PATH "shader/mesh.ps");
			effects[ind]->SetBlendState(BlendState(BlendState::BLEND_OPERATION::DISABLE, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::LESS, true));
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::FRONT, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::OBJECT_GEOMETRY));
			effects[ind]->BindTexture("diffuseTex", 0, Resources::GetSamplerState(SamplerStates::LINEAR));
			break;
		case Effects::SHIELD:
			effects[ind] = new Effect(RESOURCE_PATH "shader/shield.vs", RESOURCE_PATH "shader/shield.ps");
			effects[ind]->SetBlendState(BlendState(BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::EQUAL, false));
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::FRONT, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::OBJECT_SHIELD));
			effects[ind]->BindTexture("diffuseTex", 0, Resources::GetSamplerState(SamplerStates::LINEAR));
			effects[ind]->BindTexture("noiseTex", 1, Resources::GetSamplerState(SamplerStates::POINT));
			break;
		case Effects::DEFFERED_LIGHT:
			effects[ind] = new Effect(RESOURCE_PATH "shader/light.vs", RESOURCE_PATH "shader/light.ps", RESOURCE_PATH "shader/light.gs");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::ADD, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->BindTexture("screenTex", 0, Resources::GetSamplerState(SamplerStates::POINT));
			effects[ind]->BindTexture("depthTex", 1, Resources::GetSamplerState(SamplerStates::POINT));
			effects[ind]->BindTexture("normalTex", 2, Resources::GetSamplerState(SamplerStates::POINT));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::SIMPLE_OBJECT));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::GLOBAL));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::CAMERA));
			break;
		case Effects::DD_LIGHT:
			effects[ind] = new Effect(RESOURCE_PATH "shader/fullscreentri.vs", RESOURCE_PATH "shader/dominant_light.ps");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::ADD, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->BindTexture("screenTex", 0, Resources::GetSamplerState(SamplerStates::POINT));
			effects[ind]->BindTexture("depthTex", 1, Resources::GetSamplerState(SamplerStates::POINT));
			effects[ind]->BindTexture("normalTex", 2, Resources::GetSamplerState(SamplerStates::POINT));
			effects[ind]->BindUniformBuffer(Resources::GetUBO(UniformBuffers::DOMINANT_LIGHT));
			break;
		case Effects::DARKEN:
			effects[ind] = new Effect(RESOURCE_PATH "shader/fullscreentri.vs", RESOURCE_PATH "shader/darken.ps");
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->SetDepthStencilState(DepthStencilState(DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->SetBlendState(BlendState(BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::ZERO, BlendState::BLEND::SRC_ALPHA));
			effects[ind]->BindUniformBuffer(Resources::GetUBO(UniformBuffers::GLOBAL));
			effects[ind]->BindTexture("normalTex", 2, Resources::GetSamplerState(SamplerStates::POINT));
			break;
		case Effects::TEXTURE_2DQUAD:
			effects[ind] = new Effect(RESOURCE_PATH "shader/screentex.vs", RESOURCE_PATH "shader/screentex.ps", RESOURCE_PATH "shader/screentex.gs");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::ADD, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::INV_SRC_ALPHA));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->BindTexture("u_screenTex", 7, GetSamplerState(SamplerStates::POINT));
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			break;
		case Effects::BACKGROUNDSTAR:
			effects[ind] = new Effect(RESOURCE_PATH "shader/backgroundstar.vs", RESOURCE_PATH "shader/backgroundstar.ps", RESOURCE_PATH "shader/backgroundstar.gs");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::ADD, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::CAMERA));
			break;
		case Effects::LINES:
			effects[ind] = new Effect(RESOURCE_PATH "shader/lines.vs", RESOURCE_PATH "shader/lines.ps", RESOURCE_PATH "shader/lines.gs");
			effects[ind]->SetBlendState(BlendState(BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::LESS, false));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::OBJECT_LINES));
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			break;
		case Effects::SCREEN_OUTPUT:
			effects[ind] = new Effect(RESOURCE_PATH "shader/fullscreentri.vs", RESOURCE_PATH "shader/output.ps");
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->SetDepthStencilState(DepthStencilState(DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->BindUniformBuffer(Resources::GetUBO(UniformBuffers::GLOBAL));
			effects[ind]->BindTexture("screenTex", 0, Resources::GetSamplerState(SamplerStates::POINT));
			break;
		case Effects::BLACKHOLE:
			effects[ind] = new Effect(RESOURCE_PATH "shader/postprocessing/blackhole.vs", RESOURCE_PATH "shader/postprocessing/blackhole.ps", RESOURCE_PATH "shader/postprocessing/blackhole.gs");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::DISABLE, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::SIMPLE_OBJECT));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::GLOBAL));
			effects[ind]->BindTexture("screenTex", 0, Resources::GetSamplerState(SamplerStates::LINEAR));
			break;
		case Effects::SUN:
			effects[ind] = new Effect(RESOURCE_PATH "shader/sun.vs", RESOURCE_PATH "shader/sun.ps", RESOURCE_PATH "shader/sun.gs");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::ADD, Graphic::BlendState::BLEND::SRC_ALPHA, Graphic::BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::LESS, true)); //LESS
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::SIMPLE_OBJECT));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::GLOBAL));
			break;
		case Effects::PARTICLE_BLOB:
			effects[ind] = new Effect(RESOURCE_PATH "shader/particle/blob.vs", RESOURCE_PATH "shader/particle/blob.ps", RESOURCE_PATH "shader/particle/blob.gs");
			effects[ind]->SetBlendState(BlendState(Graphic::BlendState::BLEND_OPERATION::DISABLE, Graphic::BlendState::BLEND::ZERO, Graphic::BlendState::BLEND::ZERO));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::LESS, true));
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::SIMPLE_OBJECT));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::GLOBAL));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::CAMERA));
			break;
		case Effects::PARTICLE_RAY:
			effects[ind] = new Effect(RESOURCE_PATH "shader/particle/ray.vs", RESOURCE_PATH "shader/particle/ray.ps", RESOURCE_PATH "shader/particle/ray.gs");
			effects[ind]->SetBlendState(BlendState(BlendState::BLEND_OPERATION::MAX, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::LESS, false));
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::SIMPLE_OBJECT));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::GLOBAL));
			break;
		case Effects::PARTICLE_TEXQAUD:
			effects[ind] = new Effect(RESOURCE_PATH "shader/particle/texquad.vs", RESOURCE_PATH "shader/particle/texquad.ps", RESOURCE_PATH "shader/particle/texquad.gs");
			effects[ind]->SetBlendState(BlendState(BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE));
			effects[ind]->SetDepthStencilState(DepthStencilState(Graphic::DepthStencilState::COMPARISON_FUNC::LESS, false));
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::SIMPLE_OBJECT));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::GLOBAL));
			break;
		case Effects::BRIGHTNESS:
			effects[ind] = new Effect(RESOURCE_PATH "shader/fullscreentri.vs", RESOURCE_PATH "shader/postprocessing/brightness.ps");
			effects[ind]->SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
			effects[ind]->SetDepthStencilState(DepthStencilState(DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
			effects[ind]->SetBlendState(BlendState(BlendState::BLEND_OPERATION::DISABLE, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::ONE));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::BRIGHTNESS_PARAMS));
			effects[ind]->BindUniformBuffer(GetUBO(UniformBuffers::GLOBAL));
			effects[ind]->BindTexture("screenTex", 0, GetSamplerState(SamplerStates::LINEAR));
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
			uniformBuffers[ind]->AddAttribute("View", Graphic::UniformBuffer::ATTRIBUTE_TYPE::MATRIX);
			uniformBuffers[ind]->AddAttribute("SignDir", Graphic::UniformBuffer::ATTRIBUTE_TYPE::VEC3);
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
		case UniformBuffers::OBJECT_GEOMETRY:
			uniformBuffers[ind] = new UniformBuffer("Object");
			uniformBuffers[ind]->AddAttribute("c_WorldViewProjection", UniformBuffer::ATTRIBUTE_TYPE::MATRIX);
			uniformBuffers[ind]->AddAttribute("c_NormalWorldView", UniformBuffer::ATTRIBUTE_TYPE::MATRIX);
			break;
		case UniformBuffers::OBJECT_SHIELD:
			uniformBuffers[ind] = new UniformBuffer("Object");
			uniformBuffers[ind]->AddAttribute("c_WorldViewProjection", UniformBuffer::ATTRIBUTE_TYPE::MATRIX);
			uniformBuffers[ind]->AddAttribute("c_PointOfOrigin", UniformBuffer::ATTRIBUTE_TYPE::VEC3);
			uniformBuffers[ind]->AddAttribute("c_LocalTime", UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[ind]->AddAttribute("c_Size", UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[ind]->AddAttribute("c_Intensity", UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			break;
		case UniformBuffers::DOMINANT_LIGHT:
			uniformBuffers[ind] = new UniformBuffer("DominantDirectionLight");
			uniformBuffers[ind]->AddAttribute("Direction", UniformBuffer::ATTRIBUTE_TYPE::VEC3);
			uniformBuffers[ind]->AddAttribute("Color", UniformBuffer::ATTRIBUTE_TYPE::VEC3);
			uniformBuffers[ind]->AddAttribute("Intensity", UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			break;
		case UniformBuffers::BRIGHTNESS_PARAMS:
			uniformBuffers[ind] = new UniformBuffer("BrightnessParams");
			uniformBuffers[ind]->AddAttribute("Brightness", UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
			uniformBuffers[ind]->AddAttribute("Contrast", UniformBuffer::ATTRIBUTE_TYPE::FLOAT);
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
			Jo::Files::HDDFile file(RESOURCE_PATH "texture/defaultcontainer.sraw");
			textureMap = new Jo::Files::MetaFileWrapper(file, Jo::Files::Format::SRAW);
		}

		return *textureMap;
	}

	// ****************************************************** //
	Mesh& Resources::GetMesh(const std::string& _name, const std::string& _textureName)
	{
		auto it = meshes.find(std::pair(_name,_textureName));
		Mesh* mesh;
		if (it == meshes.end())
		{
			mesh = new Mesh(_name, _textureName);
			meshes.emplace(std::pair(_name, _textureName), mesh);
		}
		else mesh = it->second;

		return *mesh;
	}

	// ****************************************************** //
	RawMesh& Resources::GetRawMesh(const std::string& _name)
	{
		auto it = rawMeshes.find(_name);
		RawMesh* mesh;
		if (it == rawMeshes.end())
		{
			mesh = new RawMesh(RESOURCE_PATH "models/" + _name + ".wim");
			rawMeshes.emplace(_name, mesh);
		}
		else mesh = it->second;

		return *mesh;
	}

	// ****************************************************** //
	Texture& Resources::GetTexture(const std::string& _name)
	{
		auto it = textures.find(_name);
		Texture* texture;
		if (it == textures.end())
		{
			texture = new Texture(RESOURCE_PATH "texture/" + _name + ".png");
			textures.emplace(_name, texture);
		}
		else texture = it->second;

		return *texture;
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

		for (auto& mesh : meshes)
			delete mesh.second;

		for (auto& mesh : rawMeshes)
			delete mesh.second;

		for (auto& texture : textures)
			delete texture.second;
	}
}