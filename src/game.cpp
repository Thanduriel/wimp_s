#include <chrono>
#include <GLFW/glfw3.h>
#include "game.hpp"
#include "graphic/core/device.hpp"
#include "gamestates/mainstate.hpp"
#include "graphic/core/texture.hpp"

#include "utils/loggerinit.hpp"

using namespace Graphic;

Game::Game()
{
	// create context
	int screenWidth = 1366;
	int screenHeight = 768;
	Graphic::Device::Initialize(screenWidth, screenHeight, false);

	m_gameStates.emplace_back(new GameStates::MainState());

	m_sceneColorTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
		Texture::Format(4, 8, Texture::Format::ChannelType::UINT));
	m_sceneDepthTexture = new Texture(Graphic::Device::GetBackbufferSize()[0], Device::GetBackbufferSize()[1],
		Texture::Format(1, 32, Texture::Format::ChannelType::FLOAT, Texture::Format::FormatType::DEPTH));
	m_sceneFramebuffer = new Framebuffer(Framebuffer::Attachment(m_sceneColorTexture), 
		Framebuffer::Attachment(m_sceneDepthTexture));
}

void Game::Run()
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

		GameState& current = *m_gameStates.back();

		current.Process(d.count());

		// rendering
		Graphic::Device::BindFramebuffer(m_sceneFramebuffer);
		current.Draw(d.count());
		// Present if not closed
		glfwSwapBuffers(Graphic::Device::GetWindow());

		// state managment with a stack
		GameState* newState = current.GetNewState();
		if (current.IsFinished()) m_gameStates.pop_back();
		if (newState) m_gameStates.emplace_back(newState);

		glfwPollEvents();
		if (glfwWindowShouldClose(Graphic::Device::GetWindow())) m_gameStates.clear();
	}
}