#pragma once
#include <memory>

#include "graphic/core/framebuffer.hpp"
#include "gamestates/gamestate.hpp"

class Game
{
public:
	Game();
	~Game();

	void Run();
private:
	std::vector < std::unique_ptr< GameStates::GameState > > m_gameStates;

	Graphic::Texture* m_sceneDepthTexture; ///< Main depth target for the 3D scene.
	Graphic::Texture* m_sceneColorTexture; ///< Main color target for the 3D scene.
	Graphic::Framebuffer* m_sceneFramebuffer; ///< Framebuffer into which the entire 3D scene is rendered before it is passed to the postprocessing module.
};