#include "transitionstate.hpp"
#include "control/input.hpp"
#include "graphic/core/device.hpp"

namespace GameStates {
	using namespace Control;

	TransitionState::TransitionState()
	{
	}

	void TransitionState::Process(float _deltaTime)
	{
	}

	void TransitionState::Draw(float _deltaTime)
	{
		Graphic::Device::DrawFramebufferToBackbuffer();

		m_hud.Draw(_deltaTime);
	}

	// ******************************************************* //
	void TransitionState::MouseMove(float _dx, float _dy)
	{
		m_hud.MouseMove(_dx, _dy);
	}
	void TransitionState::Scroll(float _dx, float _dy)
	{
		if (m_hud.Scroll(_dx, _dy)) return;
	}
	void TransitionState::KeyDown(int _key, int _modifiers)
	{
		if (m_hud.KeyDown(_key, _modifiers)) return;
	}
	void TransitionState::KeyRelease(int _key)
	{
		if (m_hud.KeyUp(_key, 0)) return;
	}
	void TransitionState::KeyClick(int _key)
	{

	}
	void TransitionState::KeyDoubleClick(int _key)
	{

	}
}