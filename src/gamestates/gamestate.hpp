#pragma once

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

		// Returns a newly created state.
		GameState* FetchNewState() { return m_newState; m_newState = nullptr; }
		bool IsFinished() const { return m_isFinished; }
	private:
		bool m_isFinished;
		GameState* m_newState;
	};

}