#include "creditsstate.hpp"
#include "control/input.hpp"
#include "graphic/core/device.hpp"

namespace GameStates {
	using namespace Control;

	CreditsState::CreditsState()
	{
	}

	void CreditsState::Process(float _deltaTime)
	{
	}

	void CreditsState::Draw(float _deltaTime)
	{
		Graphic::Device::DrawFramebufferToBackbuffer();

		m_hud.Draw(_deltaTime);
	}

	// ******************************************************* //
	void CreditsState::MouseMove(float _dx, float _dy)
	{
		m_hud.MouseMove(_dx, _dy);
	}
	void CreditsState::Scroll(float _dx, float _dy)
	{
		if (m_hud.Scroll(_dx, _dy)) return;
	}
	void CreditsState::KeyDown(int _key, int _modifiers)
	{
		if (m_hud.KeyDown(_key, _modifiers)) return;
	}
	void CreditsState::KeyRelease(int _key)
	{
		if (m_hud.KeyUp(_key, 0)) return;
	}
	void CreditsState::KeyClick(int _key)
	{

	}
	void CreditsState::KeyDoubleClick(int _key)
	{

	}
}