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

		// input handling
		void MouseMove(float _dx, float _dy) {}
		void Scroll(float _dx, float _dy) {}
		void KeyDown(int _key, int _modifiers) {}
		void KeyRelease(int _key) {}
		void KeyClick(int _key) {}
		void KeyDoubleClick(int _key) {}

		// Returns a newly created state.
		GameState* FetchNewState() { return m_newState; m_newState = nullptr; }
		bool IsFinished() const { return m_isFinished; }
	private:
		bool m_isFinished;
		GameState* m_newState;
	};

	template<typename HudT>
	class GameStateHT : public GameState
	{
	public:
	protected:
		HudT m_hud;
	};

}