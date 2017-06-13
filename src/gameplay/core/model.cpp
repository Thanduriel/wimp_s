#include "model.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "control/camera.hpp"

namespace Game {
	
	GeometryComponent::GeometryComponent(Actor& _actor, const std::string& _pFile)
		: ActorComponent(_actor),
		Mesh(_pFile)
	{}

	void GeometryComponent::Draw()
	{
		Graphic::UniformBuffer& objectConstants = Graphic::Resources::GetUBO(Graphic::UniformBuffers::SIMPLE_OBJECT);
		ei::Mat4x4 modelViewProjection = Control::g_camera.GetViewProjection() * m_actor.GetTransformation();

		objectConstants["c_WorldViewProjection"] = modelViewProjection;

		Mesh::Draw();
	}

	Model::Model(const std::string& _pFile, const ei::Vec3&_position, const ei::Quaternion&_rotation)
		: DynamicActor(_position, _rotation),
		GeometryComponent(THISACTOR, _pFile)
	{}
}