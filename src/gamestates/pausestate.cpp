#include "pausestate.hpp"
#include "mainmenustate.hpp"
#include "settingsstate.hpp"
#include "graphic/interface/hud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "graphic/interface/hudelements.hpp"
#include "control/input.hpp"

namespace GameStates
{
	using namespace Graphic;
	using namespace Control;

	PauseState::PauseState(MainState& _oldState)
	{
		using namespace Graphic;
		m_oldState = &_oldState;
		m_hud.GetContinueButton().SetOnMouseUp([&]() { m_isFinished = true; });
		m_hud.GetOptionsButton().SetOnMouseUp([&]() { m_newState = new SettingsState(); });
		m_hud.GetMenuButton().SetOnMouseUp([&]() { m_isFinished = true; m_oldState->Finish(); });
	}

	PauseState::~PauseState()
	{
	}

	void PauseState::Process(float _deltaTime)
	{
	}

	void PauseState::Draw(float _deltaTime)
	{
		m_oldState->Draw(_deltaTime);
		m_hud.Draw(_deltaTime);
	}

	void PauseState::Dispose()
	{
	}

	// ******************************************************* //
	void PauseState::MouseMove(float _dx, float _dy)
	{
		m_hud.MouseMove(_dx, _dy);
	}
	void PauseState::Scroll(float _dx, float _dy)
	{
		if (m_hud.Scroll(_dx, _dy)) return;
	}
	void PauseState::KeyDown(int _key, int _modifiers)
	{
		if (m_hud.KeyDown(_key, _modifiers)) return;
	}
	void PauseState::KeyRelease(int _key)
	{
		if (InputManager::IsVirtualKey(_key, Control::VirtualKey::PAUSE))
			m_isFinished = true;

		if (m_hud.KeyUp(_key, 0)) return;
	}
	void PauseState::KeyClick(int _key)
	{

	}
	void PauseState::KeyDoubleClick(int _key)
	{

	}
}