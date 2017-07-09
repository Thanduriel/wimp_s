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
		// thus this operations cancel them self out
		ei::Mat4x4 normalTransform = Control::g_camera.GetView() * m_actor.GetTransformation();

		objectConstants["c_WorldViewProjection"] = modelViewProjection;
		objectConstants["c_NormalWorldView"] = normalTransform;

		m_mesh->Draw();
	}

	Model::Model(const std::string& _pFile, const ei::Vec3&_position, const ei::Quaternion&_rotation)
		: DynamicActor(_position, _rotation),
		m_geometryComponent(THISACTOR, _pFile),
		m_collisionComponent(THISACTOR, m_geometryComponent.GetMesh().GetBoundingRadius(),
			ei::Box(m_geometryComponent.GetMesh().GetLowerBound(), m_geometryComponent.GetMesh().GetUpperBound()))
	{
		// calculate inertia tensor
		const ei::Vec3& min = m_geometryComponent.GetMesh().GetLowerBound();
		const ei::Vec3& max = m_geometryComponent.GetMesh().GetUpperBound();
		float h = max.y - min.y; h *= h;
		float d = max.z - min.z; d *= d;
		float w = max.x - min.x; w *= w;
		m_inertiaTensor.m00 = 1.f / 12.f * m_mass * (h + d);
		m_inertiaTensor.m11 = 1.f / 12.f * m_mass * (w + d);
		m_inertiaTensor.m22 = 1.f / 12.f * m_mass * (w + h);
		m_inverseInertiaTensor = ei::invert(m_inertiaTensor);
	}

	void Model::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_geometryComponent);
		_sceneGraph.RegisterComponent(m_collisionComponent);
	}
}