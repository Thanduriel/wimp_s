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

		//Access thrust
		void SetThrust(const float _thrust) { m_thrust = _thrust; };
		const float GetThrust() const { return m_thrust; };

		// Access weight
		void SetWeight(const float _weight) { m_weight = _weight; }
		float GetWeight() const { return m_weight; }

		// Access speed
		void SetSpeed(const float _speed) { m_speed = _speed; }
		float GetSpeed() const { return m_speed; }

		// Access max speed
		void SetMaxSpeed(const float _maxSpeed) { m_maxSpeed = _maxSpeed; }
		float GetMaxSpeed() const { return m_maxSpeed; }
	private:
		// the power of acceleration
		float m_thrust;
		// the weight of the ship (increased weight = increased intertia)
		float m_weight;
		// the speed at which the ship will fly
		float m_speed;
		// the maximum speed of the ship
		float m_maxSpeed;
	};
}