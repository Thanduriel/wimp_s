#include "model.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "control/camera.hpp"
#include "gameplay/scenegraph.hpp"

namespace Game {
	
	GeometryComponent::GeometryComponent(const Actor& _actor, const std::string& _pFile)
		: ConstActorComponent(_actor),
		m_mesh(&Graphic::Resources::GetMesh(_pFile))
	{}

	void GeometryComponent::Draw()
	{
		Graphic::UniformBuffer& objectConstants = Graphic::Resources::GetUBO(Graphic::UniformBuffers::OBJECT_GEOMETRY);
		ei::Mat4x4 modelViewProjection = Control::g_camera.GetViewProjection() * m_actor.GetTransformation();
		// is actually the transpose(inverse(matrix)) but this only consists of rotation and translation
		// thus this operations cancel themself out
		ei::Mat4x4 normalTransform = Control::g_camera.GetView() * m_actor.GetTransformation();

		objectConstants["c_WorldViewProjection"] = modelViewProjection;
		objectConstants["c_NormalWorldView"] = normalTransform;

		m_mesh->Draw();
	}

	Model::Model(const std::string& _pFile, const ei::Vec3&_position, const ei::Quaternion&_rotation)
		: DynamicActor(_position, _rotation),
		GeometryComponent(THISACTOR, _pFile)
	{}

	void Model::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(component_cast<GeometryComponent>(*this));
	}
}