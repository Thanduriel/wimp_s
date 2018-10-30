#include "creditsstate.hpp"
#include "control/input.hpp"
#include "graphic/core/device.hpp"
#include "GLFW/glfw3.h"

namespace GameStates {
	using namespace Control;

	CreditsState::CreditsState()
		: BasicMenuState(Utils::Color32F(0.9f, 0.2f, 0.4f, 0.3f))
	{
		m_hud.m_returnButton.SetOnMouseUp([this]() {m_isFinished = true; });
	}

	void CreditsState::Process(float _deltaTime)
	{
	}

	void CreditsState::Draw(float _deltaTime)
	{
		BasicMenuState<CreditsHud>::Draw(_deltaTime);

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
		if (_key == GLFW_KEY_ESCAPE)
			m_isFinished = true;

		if (m_hud.KeyUp(_key, 0)) return;
	}
	void CreditsState::KeyClick(int _key)
	{

	}
	void CreditsState::KeyDoubleClick(int _key)
	{

	}
}