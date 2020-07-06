#pragma once

#include "graphic/interface/hud.hpp"

namespace GameStates {


	class GameState
	{
	public:
		GameState() : m_isFinished(false), m_newState(nullptr) {}
		virtual ~GameState() {};

		// Called every frame to handle game logic.
		// @param _deltaTime The time that has passed since the last frame in seconds.
		virtual void Process(float _deltaTime) = 0;

		// Called every frame to handle rendering.
		virtual void Draw(float _deltaTime) = 0;

		// Disposes the as destroyed marked actors.
		virtual void Dispose() {}

		// Called when this GameState is found on top of the stack after a state change.
		virtual void OnActivate() { MouseMove(0.f, 0.f); }

		// input handling
		virtual void MouseMove(float _dx, float _dy) {}
		virtual void Scroll(float _dx, float _dy) {}
		virtual void KeyDown(int _key, int _modifiers) {}
		virtual void KeyRelease(int _key) {}
		virtual void KeyClick(int _key) {}
		virtual void KeyDoubleClick(int _key) {}

		// Returns a newly created state.
		GameState* FetchNewState() { GameState* s = m_newState; m_newState = nullptr; return s; }
		bool IsFinished() const { return m_isFinished; }
		void Finish() { m_isFinished = true; }
	protected:
		bool m_isFinished;
		GameState* m_newState;
	};

	template<typename HudT = Graphic::Hud, 
		typename = typename std::enable_if< std::is_base_of<Graphic::Hud, HudT>::value >::type>
	class GameStateHT : public GameState
	{
	public:

		HudT& GetHud() { return m_hud; }
	protected:
		HudT m_hud;
	};

}