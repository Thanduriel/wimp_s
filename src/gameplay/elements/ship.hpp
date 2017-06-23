#pragma once

#include "gameplay/core/model.hpp"

namespace Game
{
	using namespace std;
	using namespace ei;

	class Ship : public Model
	{
	public:
		Ship(const string& _pFile, const Vec3& _position);

		void Process(float _deltaTime) override;

		// Update speed
		void UpdateSpeed(float currentSpeed, float _deltaTime);

		// Update spray radius
		void UpdateSprayRadius(float currentSpeed);

		//Access thrust
		void SetThrust(const float _thrust) { m_thrust = _thrust; };
		const float GetThrust() const { return m_thrust; };

		// Access weight
		void SetWeight(const float _weight) { m_weight = _weight; }
		float GetWeight() const { return m_weight; }

		// Access speed
		void SetSpeed(const float _speed) { m_speed = ei::max(ei::min(m_maxSpeed, _speed), m_minSpeed); }
		float GetSpeed() const { return m_speed; }

		// Access min speed
		void SetMinSpeed(const float _minSpeed) { m_minSpeed = _minSpeed; }
		float GetMinSpeed() const { return m_minSpeed; }

		// Access max speed
		void SetMaxSpeed(const float _maxSpeed) { m_maxSpeed = _maxSpeed; }
		float GetMaxSpeed() const { return m_maxSpeed; }

		// Calculate current speed
		float GetCurrentSpeed() const;

		// Access max spray radius
		void SetMaxSprayRadius(const float _maxSprayRadius) { m_maxSprayRadius = _maxSprayRadius; }
		float GetMaxSprayRadius() const { return m_maxSprayRadius; }

		// Access min spray radius
		void SetMinSprayRadius(const float _minSprayRadius) { m_minSprayRadius = _minSprayRadius; }
		float GetMinSprayRadius() const { return m_minSprayRadius; }

		// Access current spray radius
		float GetSprayRadius() const { return m_sprayRadius; }
	private:
		// the power of acceleration
		float m_thrust;
		// the weight of the ship (increased weight = increased intertia)
		float m_weight;
		// the speed at which the ship will fly
		float m_speed;
		// the minimum speed of the ship
		float m_minSpeed;
		// the maximum speed of the ship
		float m_maxSpeed;
		// min spray radius
		float m_minSprayRadius;
		// max spray radius
		float m_maxSprayRadius;
		// the spray radius
		float m_sprayRadius;
	};
}