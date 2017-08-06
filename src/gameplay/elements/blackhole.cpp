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

	void BlackHoleComponent::SetRadius(float _radius)
	{
		m_renderer.Add(ei::Vec3(0.f), _radius);
	}

//	const float G = 6.67408-e11;
	const float STRENGTH_TRESHHOLD = 100.f;
	BlackHoleGravitationComponent::BlackHoleGravitationComponent(const Actor& _actor, float _radius, float _strength)
		: SceneComponent(_actor),
		m_radius(_radius),
		m_fact(_strength * _radius * _radius)
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
			float f = m_fact / p.second;

			dynAct->AddVelocity((m_actor.GetPosition() - dynAct->GetPosition()) / sqrt(p.second) * f * _deltaTime);
		}
	}

	// radius of the event horizon as ratio to _radius
	const float EVENT_HORIZON = 0.1f;
	BlackHole::BlackHole(const ei::Vec3& _position, float _radius, float _strength, float _duration)
		: Actor(_position),
		m_gravitationComponent(THISACTOR, _radius, _strength),
		m_visualComponent(THISACTOR, EVENT_HORIZON * _radius),
		m_lifeTime(THISACTOR, _duration),
		m_collisionComponent(THISACTOR, EVENT_HORIZON * _radius, CollisionComponent::Type::Any)
	{}

	void BlackHole::Process(float _deltaTime)
	{
		Actor::Process(_deltaTime);

		// decay visually 
		if (m_lifeTime.GetLifeTimeLeft() < 1.f)
			m_visualComponent.SetRadius(m_collisionComponent.GetBoundingRadius() * m_lifeTime.GetLifeTimeLeft());

		m_deltaTime = _deltaTime;
	}

	void BlackHole::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_gravitationComponent);
		_sceneGraph.RegisterComponent(m_visualComponent);
		_sceneGraph.RegisterComponent(m_lifeTime);
		_sceneGraph.RegisterComponent(m_collisionComponent);
	}

	const float DAMAGE_PER_SECOND = 0.2f;
	void BlackHole::OnCollision(Actor& _other)
	{
		_other.Damage(DAMAGE_PER_SECOND * _other.GetMaxHealth() * m_deltaTime, *this);
	}
}