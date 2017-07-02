#include <chrono>
#include <GLFW/glfw3.h>
#include "game.hpp"
#include "graphic/core/device.hpp"
#include "gamestates/mainstate.hpp"
#include "graphic/core/texture.hpp"
#include "graphic/resources.hpp"
#include "control/input.hpp"
#include "graphic/effects/lightsystem.hpp"
#include "graphic/effects/particlesystem.hpp"

#include "utils/loggerinit.hpp"

using namespace Graphic;

float Wimp_s::m_gameTime;

Wimp_s::Wimp_s()
{
	// Load configuration
	try {
		Jo::Files::HDDFile file("config.json");
		m_config.Read(file, Jo::Files::Format::JSON);
	}
	catch (std::string _message) {
		LOG_ERROR("Failed to load config file with message:\n" + _message + '\n' + "\nLoading default configuration.");
		BuildDefaultConfig();

		try {
			Jo::Files::HDDFile file("config.json", Jo::Files::HDDFile::OVERWRITE);
			m_config.Write(file, Jo::Files::Format::JSON);
		}
		catch (std::string _message) {
			LOG_ERROR("Failed to write config file with message:\n" + _message + '\n' + "\n");
		}
	}

	// create context
	const int screenWidth = 1366;
	const int screenHeight = 768;
	Graphic::Device::Initialize(screenWidth, screenHeight, false);

	m_sceneColorTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
		Texture::Format(4, 8, Texture::Format::ChannelType::UINT));
	m_sceneNormalTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
		Texture::Format(4, 32, Texture::Format::ChannelType::FLOAT));
	m_sceneDepthTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
		Texture::Format(1, 32, Texture::Format::ChannelType::FLOAT, Texture::Format::FormatType::DEPTH));
	m_sceneFramebuffer = new Framebuffer({ Framebuffer::Attachment(m_sceneColorTexture), Framebuffer::Attachment(m_sceneNormalTexture)},
		Framebuffer::Attachment(m_sceneDepthTexture));

	Control::InputManager::Initialize(Graphic::Device::GetWindow(), m_config[std::string("Input")]);//config[std::string("Input")]

	LightSystem::Initialize();

	m_gameStates.emplace_back(new GameStates::MainState());
}

Wimp_s::~Wimp_s()
{
	Control::InputManager::Close();
	LightSystem::Close();
	ParticleSystems::Manager::Close();

	delete m_sceneColorTexture;
	delete m_sceneNormalTexture;
	delete m_sceneDepthTexture;
	delete m_sceneFramebuffer;
	Graphic::Resources::Unload();

	Device::Exit();
}

void Wimp_s::Run()
{
	using namespace GameStates;
	using namespace std::chrono;

	steady_clock::time_point begin = steady_clock::now();

	while (m_gameStates.size())
	{
		// messure delta time
		steady_clock::time_point end = steady_clock::now();
		duration<float> d = duration_cast<duration<float>>(end - begin);
		begin = end;
		m_gameTime += d.count();

		GameState& current = *m_gameStates.back();

		current.Process(d.count());

		// rendering
		// use hardware backbuffer for now
		Graphic::Device::BindFramebuffer( m_sceneFramebuffer );
		Graphic::Device::Clear(0.f, 0.f, 0.f);
		current.Draw(d.count());

		// Present if not closed
		glfwSwapBuffers(Graphic::Device::GetWindow());

		// delete all marked actors in the scene
		current.Dispose();

		// state managment with a stack
		GameState* newState = current.FetchNewState();
		if (current.IsFinished()) m_gameStates.pop_back();
		if (newState) m_gameStates.emplace_back(newState);

		// todo: move this when general input handling is implemented
		if (m_gameStates.size() && glfwGetKey(Graphic::Device::GetWindow(), GLFW_KEY_ESCAPE)) 
			m_gameStates.pop_back();
		if (m_gameStates.size())
		{
			Control::InputManager::SetGameState(m_gameStates.back().get());
			glfwPollEvents();
		}
		if (glfwWindowShouldClose(Graphic::Device::GetWindow())) m_gameStates.clear();
	}
}

void Wimp_s::BuildDefaultConfig()
{
	auto& cinput = m_config[std::string("Input")];
	cinput[std::string("AccForward")][0] = GLFW_KEY_W;
	cinput[std::string("AccBackward")][0] = GLFW_KEY_S;
	cinput[std::string("AccLeft")][0] = GLFW_KEY_A;
	cinput[std::string("AccRight")][0] = GLFW_KEY_D;
	cinput[std::string("AdjustTargetAcc")][0] = GLFW_KEY_LEFT_SHIFT;
	cinput[std::string("Fire")][0] = GLFW_MOUSE_BUTTON_LEFT;
	cinput[std::string("RollCW")][0] = GLFW_KEY_E;
	cinput[std::string("RollCCW")][0] = GLFW_KEY_Q;
	
/*	auto& cgame = m_config[std::string("Game")];
	cgame[std::string("Language")] = "english.json";

	auto& cgraphics = Config[std::string("Graphics")];
	cgraphics[std::string("ScreenWidth")] = 1366;
	cgraphics[std::string("ScreenHeight")] = 768; */
}