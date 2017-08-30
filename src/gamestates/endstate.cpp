#include "endstate.hpp"
#include "control/input.hpp"
#include "graphic/core/device.hpp"

namespace GameStates {
	using namespace Control;

	EndState::EndState()
	{
	}

	void EndState::Process(float _deltaTime)
	{
	}

	void EndState::Draw(float _deltaTime)
	{
		Graphic::Device::DrawFramebufferToBackbuffer();

		m_hud.Draw(_deltaTime);
	}

	// ******************************************************* //
	void EndState::MouseMove(float _dx, float _dy)
	{
		m_hud.MouseMove(_dx, _dy);
	}
	void EndState::Scroll(float _dx, float _dy)
	{
		if (m_hud.Scroll(_dx, _dy)) return;
	}
	void EndState::KeyDown(int _key, int _modifiers)
	{
		if (m_hud.KeyDown(_key, _modifiers)) return;
	}
	void EndState::KeyRelease(int _key)
	{
		if (m_hud.KeyUp(_key, 0)) return;
	}
	void EndState::KeyClick(int _key)
	{

	}
	void EndState::KeyDoubleClick(int _key)
	{

	}
}