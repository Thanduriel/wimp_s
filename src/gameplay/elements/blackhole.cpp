#include "blackhole.hpp"
#include "control/camera.hpp"

namespace Game {
	void BlackHole::Draw()
	{
		m_renderer.Add(ei::Vec3(0.f), 5.f);
		m_renderer.Draw(Control::g_camera.GetViewProjection() * m_transformation);
	}
}