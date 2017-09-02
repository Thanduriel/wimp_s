#include "sun.hpp"
#include "control/camera.hpp"

namespace Game {

	SunComponent::SunComponent(const Actor& _actor, float _radius)
		: MarkerComponent(_actor)
	{
		m_renderer.Add(_actor.GetPosition(), _radius);
	}

	void SunComponent::Draw()
	{
		m_renderer.Draw(Control::g_camera.GetViewProjection() * m_actor.GetTransformation());
	}

	Sun::Sun(const ei::Vec3& _position, float _radius)
		: Actor(_position),
		m_sunComponent(THISACTOR, _radius)
	//	m_lightComponent(THISACTOR, _position * -0.8f, 2000.f, Utils::Color8U(1.f, 1.f, 1.f), 0.9f)
	{}

	void Sun::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_sunComponent);
	//	_sceneGraph.RegisterComponent(m_lightComponent);
	}
}