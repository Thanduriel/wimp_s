#include "blackhole.hpp"
#include "control/camera.hpp"

namespace Game {
	void BlackHoleComponent::Draw()
	{
		m_renderer.Add(ei::Vec3(0.f), 5.f);
		m_renderer.Draw(Control::g_camera.GetViewProjection() * m_actor.GetTransformation());
	}

	BlackHole::BlackHole(const ei::Vec3& _position, float _radius)
		: Actor(_position),
		BlackHoleComponent(THISACTOR)
	{

	}
}