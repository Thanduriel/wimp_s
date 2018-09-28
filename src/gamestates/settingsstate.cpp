#include "settingsstate.hpp"
#include "control/input.hpp"
#include "GLFW/glfw3.h"
#include "graphic/core/device.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "control/playercontroller.hpp"
#include "game.hpp"
#include "gameplay/elements/audiocomponent.hpp"

namespace GameStates {
	using namespace Control;

	using namespace std::string_literals;

	const std::array<std::pair<ei::IVec2,std::string>, 3> SUPPORTED_RESOLUTIONS = 
	{ {
		{ei::IVec2(1366, 768), "   1366 x 768   "},
		{ei::IVec2(1920, 1080), "   1920 x 1080  "},
		{ei::IVec2(2560, 1440), "   2560 x 1440  "}
	} };

	SettingsState::SettingsState()
		: m_enableAimAssist(PlayerController::HAS_AIM_ASSIST)
	{
		Jo::Files::MetaFileWrapper& config = Wimp_s::GetConfig();
		auto& cgraphics = config["Graphics"s];

		// load current values and display
		std::string s = std::to_string(cgraphics["TargetFPS"s].Get(144.f));
		s.resize(5);
		m_hud.m_frameCountTarget->SetText(s);

		s = std::to_string(InputManager::GetMouseSensitivity());
		s.resize(5);
		m_hud.m_mouseSensitivity->SetText(s);

		s = std::to_string(cgraphics["Brightness"s].Get(0.f));
		s.resize(5);
		m_hud.m_brightness->SetText(s);

		s = std::to_string(cgraphics["Contrast"s].Get(1.f));
		s.resize(5);
		m_hud.m_contrast->SetText(s);

		// volume
		s = std::to_string(Game::AudioSystem::GetVolume());
		s.resize(5);
		m_hud.m_masterVolume->SetText(s);

		// aim assist
		m_hud.m_aimAssistButton->SetCaption(m_enableAimAssist ? "aim assist: ON " : "aim assist: OFF");
		m_hud.m_aimAssistButton->SetOnMouseUp([this]() 
		{
			m_enableAimAssist = !m_enableAimAssist;

			m_hud.m_aimAssistButton->SetCaption(m_enableAimAssist ? " aim assist: ON " : " aim assist: OFF");
		});

		// full screen
		m_isFullScreen = cgraphics["FullScreen"s].Get(false);
		m_hud.m_fullScreenButton->SetCaption(m_isFullScreen ? "full screen: ON " : "full screen: OFF");
		m_hud.m_fullScreenButton->SetOnMouseUp([this]()
		{
			m_isFullScreen = !m_isFullScreen;

			m_hud.m_fullScreenButton->SetCaption(m_isFullScreen ? "full screen: ON " : "full screen: OFF");
		});

		// resolution; find which option is currently active
		m_currentResolution = -1;
		const ei::IVec2 currentRes = ei::IVec2(cgraphics["ScreenWidth"s].Get(1366), cgraphics["ScreenHeight"s].Get(768));
		for (int i = 0; i < (int)SUPPORTED_RESOLUTIONS.size(); ++i)
			if (SUPPORTED_RESOLUTIONS[i].first == currentRes)
			{
				m_currentResolution = i;
				break;
			}
		// custom resolution detected
		if (m_currentResolution == -1) m_hud.m_resolutionButton->SetCaption(std::to_string(currentRes.x) + " x " + std::to_string(currentRes.y));
		else m_hud.m_resolutionButton->SetCaption(SUPPORTED_RESOLUTIONS[m_currentResolution].second);
		m_hud.m_resolutionButton->SetOnMouseUp([this]()
		{
			m_currentResolution = (m_currentResolution + 1) % SUPPORTED_RESOLUTIONS.size();
			m_hud.m_resolutionButton->SetCaption(SUPPORTED_RESOLUTIONS[m_currentResolution].second);
		});

		// close
		m_hud.m_applyButton->SetOnMouseUp([&]() 
		{
			// write back new values
			// mouse sensitivity, catch invalid input
			try {
				float f = std::stof(m_hud.m_mouseSensitivity->GetText());
				InputManager::SetMouseSensitivtiy(std::clamp(f, 0.1f, 10.f));

				f = std::stof(m_hud.m_masterVolume->GetText());
				Game::AudioSystem::SetVolume(f);

				// brightness params can be updated live
				const float brightness = std::stof(m_hud.m_brightness->GetText());
				cgraphics["Brightness"s] = (double)brightness;

				const float contrast = std::stof(m_hud.m_contrast->GetText());
				cgraphics["Contrast"s] = (double)contrast;

				Graphic::UniformBuffer& ubo = Graphic::Resources::GetUBO(Graphic::UniformBuffers::BRIGHTNESS_PARAMS);
				ubo["Brightness"] = brightness;
				ubo["Contrast"] = contrast;

				f = std::stof(m_hud.m_frameCountTarget->GetText());
				f = std::clamp(f, 30.f, 10000.f);
				cgraphics["TargetFPS"s] = (double)std::clamp(f, 30.f, 10000.f);
				Wimp_s::SetTargetFPS(f);

				cgraphics["FullScreen"s] = m_isFullScreen;
				if (m_currentResolution != -1)
				{
					cgraphics["ScreenWidth"s] = SUPPORTED_RESOLUTIONS[m_currentResolution].first.x;
					cgraphics["ScreenHeight"s] = SUPPORTED_RESOLUTIONS[m_currentResolution].first.y;
				}

			} catch(std::invalid_argument)
			{ }
			PlayerController::HAS_AIM_ASSIST = m_enableAimAssist;

			m_isFinished = true;
		});
		m_hud.m_cancelButton->SetOnMouseUp([this]() {m_isFinished = true; });
	}

	void SettingsState::Process(float _deltaTime)
	{
		BasicMenuState<SettingsHud>::Process(_deltaTime);
	}

	void SettingsState::Draw(float _deltaTime)
	{
		BasicMenuState<SettingsHud>::Draw(_deltaTime);

		Graphic::Device::DrawFramebufferToBackbuffer();

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