#pragma once

#include "ei/vector.hpp"
#include <ei/quaternion.hpp>
#include <limits>

namespace Game {

	class SpecialMove
	{
	public:
		SpecialMove(float _cooldown, float _range = std::numeric_limits<float>::max());

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
		virtual void TestPlacement(const class SceneGraph& _sceneGraph, const class Ship& _instigator) {}

		void SetIndicator(const ei::Vec3& _position, const ei::Quaternion& _rotation = ei::qidentity());

		State GetState() const { return m_state; }

	protected:
		virtual void OnTarget() {}
		virtual void OnActivate() = 0;
		virtual void OnDismiss() {}

		bool IsInRange(const ei::Vec3& _origin) const;

		float m_cooldown;
		float m_cooldownMax;
		float m_rangeSq;
		float m_range;
		State m_state;
		class Actor* m_indicator;
	};

	class BlackHoleGenerator : public SpecialMove
	{
	public:
		BlackHoleGenerator(float _cooldown, float _range = 800.f)
			: SpecialMove(_cooldown, _range)
		{}

		void TestPlacement(const class SceneGraph& _sceneGraph, const class Ship& _instigator) override;
	protected:
		void OnTarget() override;
		void OnActivate() override;
		void OnDismiss() override;
	};
}