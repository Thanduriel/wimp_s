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

	GeometryComponent::GeometryComponent(const Actor& _actor, const GeometryComponent& _orig)
		: ConstActorComponent(_actor),
		m_mesh(_orig.m_mesh)
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

	// ********************************************************************** //
	namespace Details {
		ModelComponentImpl::ModelComponentImpl(Actor& _actor, const std::string& _meshName)
			: GeometryComponent(_actor, _meshName),
			CollisionComponent(_actor, GetMesh().GetMeshBounds().boundingRadius,
				ei::Box(GetMesh().GetMeshBounds().lowerBound, GetMesh().GetMeshBounds().upperBound),
				CollisionComponent::Type::Any | CollisionComponent::Type::Solid | CollisionComponent::Type::Dynamic)
		{
		}

		ModelComponentImpl::ModelComponentImpl(Actor& _actor, const std::string& _meshName, const std::string& _boundingMeshName)
			: GeometryComponent(_actor, _meshName),
			CollisionComponent(_actor, _boundingMeshName, CollisionComponent::Type::Any | CollisionComponent::Type::Solid | CollisionComponent::Type::Dynamic)
		{
		}

		ModelComponentImpl::ModelComponentImpl(Actor& _actor, const ModelComponentImpl& _other)
			: GeometryComponent(_actor, _other),
			CollisionComponent(_actor, _other)
		{}
	}

	// ********************************************************************** //
	Model::Model(const std::string& _pFile, const ei::Vec3&_position, const ei::Quaternion&_rotation, float _mass)
		: DynamicActor(_position, _rotation),
		m_component(THISACTOR, _pFile)
	{
		m_mass = _mass;
		SetInertiaTensor(m_component.ComputeInertiaTensor(m_mass));
	}

	Model::Model(const std::string& _meshName, const std::string& _boundingMeshName, const ei::Vec3&_position, const ei::Quaternion&_rotation, float _mass)
		: DynamicActor(_position, _rotation),
		m_component(THISACTOR, _meshName, _boundingMeshName)
	{
		m_mass = _mass;
		SetInertiaTensor(m_component.ComputeInertiaTensor(m_mass));
	}

	Model::Model(const Model& _orig)
		: DynamicActor(_orig),
		m_component(THISACTOR, _orig.m_component)
	{
	}

	void Model::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_component);
	}
}