#pragma once

namespace GameStates {

	class GameState
	{
	public:
		GameState() : m_isFinished(false), m_newState(nullptr) {}
		virtual ~GameState() {};

		virtual void Process(float _deltaTime) = 0;
		virtual void Draw(float _deltaTime) = 0;

		GameState* GetNewState() const { return m_newState; }
		bool IsFinished() const { return m_isFinished; }
	private:
		bool m_isFinished;
		GameState* m_newState;
	};

}