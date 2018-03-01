#include "shieldcomponent.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "control/camera.hpp"
#include "game.hpp"

namespace Game {
	ShieldComponent::ShieldComponent(const Actor& _actor, const Graphic::Mesh& _mesh)
		: ConstActorComponent(_actor),
		m_mesh(_mesh),
		m_localTime(0.f),
		m_maxTime(1.f / std::sqrt(_mesh.GetMeshBounds().boundingRadius)),
		m_size(std::sqrt(_mesh.GetMeshBounds().boundingRadius)),
		m_speedMod(1.f / _mesh.GetMeshBounds().boundingRadius)
	{
	//	m_canTick = false;
	}

	const float LINE_SPEED_MOD = 0.15f;
	void ShieldComponent::ProcessComponent(float _deltaTime)
	{
		m_localTime += _deltaTime * m_speedMod;
		if (m_localTime > m_maxTime) m_localTime = 0.f;
	}

	void ShieldComponent::Draw()
	{
		Graphic::UniformBuffer& objectConstants = Graphic::Resources::GetUBO(Graphic::UniformBuffers::OBJECT_SHIELD);
		ei::Mat4x4 modelViewProjection = Control::g_camera.GetViewProjection() * m_actor.GetTransformation();

		objectConstants["c_WorldViewProjection"] = modelViewProjection;
		objectConstants["c_PointOfOrigin"] = ei::Vec3(0.f);
		objectConstants["c_LocalTime"] = m_localTime/* * m_localTime*/; // squared time is compared with squared distance
		objectConstants["c_Size"] = m_size;

		m_mesh.Draw();
	}
}