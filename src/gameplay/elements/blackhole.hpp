#pragma once

#include "gameplay/core/singlecomponentactor.hpp"
#include "graphic/effects/blackholerenderer.hpp"

namespace Game
{
	class BlackHoleComponent : public PostProcessComponent
	{
	public:
		BlackHoleComponent(const Actor& _actor, float _radius);

		void Draw() override;
	private:
		Graphic::BlackHoleRenderer m_renderer;
	};
	
	class BlackHoleGravitationComponent : public SceneComponent
	{
	public:
		BlackHoleGravitationComponent(const Actor& _actor, float _radius);
		void ProcessComponent(float _deltaTime, const class SceneGraph& _sceneGraph) override;
	private:
		float m_radius;
	};

//	typedef SingleComponentActor<BlackHoleComponent> BlackHoleImpl;

	class BlackHole : public Actor
	{
	public:
		BlackHole(const ei::Vec3& _position, float _radius);

		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	private:
		BlackHoleGravitationComponent m_gravitationComponent;
		BlackHoleComponent m_visualComponent;
	};
}