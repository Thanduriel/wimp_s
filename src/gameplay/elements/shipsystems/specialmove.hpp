#pragma once

#include "ei/vector.hpp"

namespace Game {

	class SpecialMove
	{
	public:
		SpecialMove(float _cooldown);

		void Process(float _deltaTime);

		enum struct State
		{
			Charging,
			Ready,
			Targeting,
			Invalid,
			Active
		};

		// set targeting creating range indicators
		void Target();
		
		// active the special move
		void Activate();

		// leave targeting without placement
		void Dismiss();

		// @return Is the current position of the indicator valid
		virtual void TestPlacement(const class SceneGraph& _sceneGraph) {}

		void SetIndicator(const ei::Vec3& _position, const ei::Quaternion& _rotation = ei::qidentity());

		State GetState() const { return m_state; }

	protected:
		virtual void OnTarget() {}
		virtual void OnActivate() = 0;
		virtual void OnDismiss() {}

		float m_cooldown;
		float m_cooldownMax;
		State m_state;
		class Actor* m_indicator;
	};

	class BlackHoleGenerator : public SpecialMove
	{
	public:
		using SpecialMove::SpecialMove;

		void TestPlacement(const class SceneGraph& _sceneGraph) override;
	protected:
		void OnTarget() override;
		void OnActivate() override;
		void OnDismiss() override;
	};
}