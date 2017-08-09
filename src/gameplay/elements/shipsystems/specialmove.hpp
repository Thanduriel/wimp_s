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
			Active
		};

		// set targeting creating range indicators
		void Target();
		
		// active the special move
		void Activate();

		// leave targeting without placement
		void Dismiss();

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
	protected:
		void OnTarget() override;
		void OnActivate() override;
		void OnDismiss() override;
	};
}