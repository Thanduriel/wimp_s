#include "blackhole.hpp"
#include "control/camera.hpp"
#include "gameplay/scenegraph.hpp"

namespace Game {
	BlackHoleComponent::BlackHoleComponent(const Actor& _actor, float _radius)
		: PostProcessComponent(_actor)
	{
		m_canTick = false;

		m_renderer.Add(ei::Vec3(0.f), _radius);
	}

	void BlackHoleComponent::Draw()
	{
		m_renderer.Draw(Control::g_camera.GetViewProjection() * m_actor.GetTransformation());
	}


//	const float G = 6.67408-e11;
	const float PULL_TRESHHOLD = 0.5;
	BlackHoleGravitationComponent::BlackHoleGravitationComponent(const Actor& _actor, float _radius)
		: SceneComponent(_actor),
		m_radius(_radius)
	{
		// no regular tick
		m_canTick = false;
	}

	void BlackHoleGravitationComponent::ProcessComponent(float _deltaTime, const SceneGraph& _sceneGraph)
	{
		auto actors = _sceneGraph.SphereQuery(ei::Sphere(m_actor.GetPosition(), m_radius), 
			CollisionComponent::Type::Dynamic);

		for (auto& p : actors)
		{
			DynamicActor* dynAct = static_cast<DynamicActor*>(p.first);
			float f = 10000.f / p.second;

			dynAct->AddVelocity((m_actor.GetPosition() - dynAct->GetPosition()) / sqrt(p.second) * f * _deltaTime);
		}
	}

	BlackHole::BlackHole(const ei::Vec3& _position, float _radius)
		: Actor(_position),
		m_gravitationComponent(THISACTOR, _radius),
		m_visualComponent(THISACTOR, 0.1f * _radius)
	{}

	void BlackHole::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_gravitationComponent);
		_sceneGraph.RegisterComponent(m_visualComponent);
	}
}