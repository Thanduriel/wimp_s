#include "settingsstate.hpp"
#include "control/input.hpp"
#include "GLFW/glfw3.h"
#include "graphic/core/device.hpp"

namespace GameStates {
	using namespace Control;

	SettingsState::SettingsState()
	{
		m_hud.m_applyButton->SetOnMouseUp([this]() { m_isFinished = true;});
		m_hud.m_cancelButton->SetOnMouseUp([this]() {m_isFinished = true; });
	}

	void SettingsState::Process(float _deltaTime)
	{
	}

	void SettingsState::Draw(float _deltaTime)
	{
		using namespace Graphic;
		Texture& tex = *Device::GetCurrentFramebufferBinding()->GetColorAttachments().begin()->pTexture;

		Device::BindFramebuffer(nullptr);
		Device::SetEffect(Graphic::Resources::GetEffect(Effects::SCREEN_OUTPUT));
		Device::SetTexture(tex, 0);
		Device::DrawFullscreen();

		m_hud.Draw(_deltaTime);
	}

	void SettingsState::Dispose()
	{
	}

	// ******************************************************* //
	void SettingsState::MouseMove(float _dx, float _dy)
	{
		m_hud.MouseMove(_dx, _dy);
	}
	void SettingsState::Scroll(float _dx, float _dy)
	{
		if (m_hud.Scroll(_dx, _dy)) return;
	}
	void SettingsState::KeyDown(int _key, int _modifiers)
	{
		if (m_hud.KeyDown(_key, _modifiers)) return;
	}
	void SettingsState::KeyRelease(int _key)
	{
		if (_key == GLFW_KEY_ESCAPE)
			m_isFinished = true;

		if (m_hud.KeyUp(_key, 0)) return;
	}
	void SettingsState::KeyClick(int _key)
	{

	}
	void SettingsState::KeyDoubleClick(int _key)
	{

	}
}