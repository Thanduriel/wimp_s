#include "blackhole.hpp"
#include "control/camera.hpp"
#include "gameplay/scenegraph.hpp"

namespace Game {
	BlackHoleComponent::BlackHoleComponent(const Actor& _actor, float _radius)
		: PostProcessComponent(_actor)
	{
		m_canTick = false;
		m_isActive = true;
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
	BlackHoleGravitationComponent::BlackHoleGravitationComponent(Actor& _actor, float _radius, float _strength)
		: SceneComponent(_actor),
		m_radius(_radius),
		m_fact(_strength * _radius * _radius)
	{
		// no regular tick
		m_canTick = false;

		// no dynamic behavior
		m_isActive = false;
	}

	void BlackHoleGravitationComponent::ProcessComponent(float _deltaTime, const SceneGraph& _sceneGraph)
	{
		auto actors = _sceneGraph.SphereQuery(ei::Sphere(m_actor.GetPosition(), m_radius), 
			CollisionComponent::Type::Dynamic);

		for (auto& p : actors)
		{
			Assert(p.second != 0.f, "Do not spawn a black hole directly on something else!");
			DynamicActor* dynAct = static_cast<DynamicActor*>(p.first);
			float f = m_fact / p.second;

			dynAct->AddVelocity((m_actor.GetPosition() - dynAct->GetPosition()) / sqrt(p.second) * f * _deltaTime);
		}
	}

	// radius of the event horizon as ratio to _radius
	const float EVENT_HORIZON = 0.12f;
	const Utils::Color32F BASE_GRID_COLOR = Utils::Color32F(0.2f, 0.85f, 0.4f, 0.7f);
	const Utils::Color32F INVALID_GRID_COLOR = Utils::Color32F(1.0f, 0.2f, 0.2f, 0.7f);
	BlackHole::BlackHole(const ei::Vec3& _position, float _radius, float _strength, float _duration)
		: Actor(_position),
		m_gravitationComponent(THISACTOR, _radius, _strength),
		m_visualComponent(THISACTOR, EVENT_HORIZON * _radius),
		m_lifeTime(THISACTOR, _duration),
		m_grid(THISACTOR, BASE_GRID_COLOR, 32, 16, _radius),
		m_collisionComponent(THISACTOR, EVENT_HORIZON * _radius, CollisionComponent::Type::Any), // no collision at first
		m_deltaTime(0.f) // when instantly activated the delta time will not be set otherwise
	{}

	void BlackHole::Process(float _deltaTime)
	{
		Actor::Process(_deltaTime);

		// decay visually 
		if (m_lifeTime.GetLifeTimeLeft() < 1.f)
			m_visualComponent.SetRadius(m_collisionComponent.GetBoundingRadius() * m_lifeTime.GetLifeTimeLeft());

		m_deltaTime = _deltaTime;
		m_isColliding = false;
	}

	void BlackHole::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_gravitationComponent);
		_sceneGraph.RegisterComponent(m_visualComponent);
		_sceneGraph.RegisterComponent(m_lifeTime);
		_sceneGraph.RegisterComponent(m_grid);
		_sceneGraph.RegisterComponent(m_collisionComponent);
	}

	const float DAMAGE_PER_SECOND = 0.43f;
	void BlackHole::OnCollision(Actor& _other)
	{
		m_isColliding = true;
		if(m_gravitationComponent.IsActive())
			_other.Damage(DAMAGE_PER_SECOND * _other.GetMaxHealth() * m_deltaTime, *this, DamageType::Pure);
	}

	void BlackHole::Activate()
	{
		m_gravitationComponent.SetActive(true);
		m_collisionComponent.SetType(CollisionComponent::Type::Any);
		m_grid.SetColor(Utils::Color32F(0.1f, 0.1f, 0.9f, 0.18f));
	}

	void BlackHole::SetInvalid(bool _inv)
	{
		m_grid.SetColor(_inv ? INVALID_GRID_COLOR : BASE_GRID_COLOR);
	}
}