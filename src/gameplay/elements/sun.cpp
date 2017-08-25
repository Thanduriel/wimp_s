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
}