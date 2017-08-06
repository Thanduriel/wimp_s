#pragma once

#include "gameplay/core/singlecomponentactor.hpp"
#include "graphic/effects/blackholerenderer.hpp"
#include "lifetimecomponent.hpp"
#include "collisioncomponent.hpp"

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
		BlackHoleGravitationComponent(const Actor& _actor, float _radius, float _strength);
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
		void OnCollision(Actor& _other) override;
	private:
		float m_deltaTime; // current delta time is stored to apply damage over time on collision

		BlackHoleGravitationComponent m_gravitationComponent;
		BlackHoleComponent m_visualComponent;
		LifeTimeComponent m_lifeTime;
		CollisionComponent m_collisionComponent; // the event horizon
	};
}