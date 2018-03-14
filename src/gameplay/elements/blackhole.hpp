#pragma once

#include "gameplay/core/singlecomponentactor.hpp"
#include "graphic/effects/blackholerenderer.hpp"
#include "lifetimecomponent.hpp"
#include "collisioncomponent.hpp"
#include "grid.hpp"

namespace Game
{
	class BlackHoleComponent : public PostProcessComponent
	{
	public:
		BlackHoleComponent(const Actor& _actor, float _radius);

		void Draw() override;

		void SetRadius(float _radius);
	private:
		Graphic::BlackHoleRenderer m_renderer;
	};

	typedef SingleComponentActor<BlackHoleComponent> BlackHoleVis;
	
	class BlackHoleGravitationComponent : public SceneComponent
	{
	public:
		BlackHoleGravitationComponent(Actor& _actor, float _radius, float _strength);
		void ProcessComponent(float _deltaTime, const class SceneGraph& _sceneGraph) override;
	private:

		float m_radius;
		float m_fact;
	};

//	typedef SingleComponentActor<BlackHoleComponent> BlackHoleImpl;

	class BlackHole : public Actor
	{
	public:
		// @param _radius Radius in which other actors are effected.
		// @param _strength Force in [N] that is applied to an object in _radius distance.
		BlackHole(const ei::Vec3& _position, float _radius, float _strength, float _duration);

		void Process(float _deltaTime) override;

		void RegisterComponents(class SceneGraph& _sceneGraph) override;
		void OnCollision(Actor& _other, const HitInfo& _info) override;

		// When created the blackhole has no gravitation or collision.
		void Activate();
		bool IsColliding() const { return m_isColliding; }
		void SetInvalid(bool _inv);

		float GetEventHorizonRadius() const { return m_collisionComponent.GetBoundingRadius(); }
	private:
		float m_deltaTime; // current delta time is stored to apply damage over time on collision
		bool m_isColliding; // some ship is partly inside the event horizon

		BlackHoleGravitationComponent m_gravitationComponent;
		BlackHoleComponent m_visualComponent;
		LifeTimeComponent m_lifeTime;
		GridComponent m_grid; // indicator to improve visibility
		CollisionComponent m_collisionComponent; // the event horizon
	};
}