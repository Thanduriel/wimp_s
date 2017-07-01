#include "blackhole.hpp"
#include "control/camera.hpp"

namespace Game {
	BlackHoleComponent::BlackHoleComponent(const Actor& _actor, float _radius)
		: PostProcessComponent(_actor)
	{
		m_renderer.Add(ei::Vec3(0.f), _radius);
	}

	void BlackHoleComponent::Draw()
	{
		m_renderer.Draw(Control::g_camera.GetViewProjection() * m_actor.GetTransformation());
	}
}