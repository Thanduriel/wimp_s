#pragma once
#include "ship.hpp"

namespace Game
{

	class EnemyShip : public Ship
	{
	public:
		EnemyShip(const string& _pFile, const Vec3& _position, Ship& _target);

		void Process(float _deltaTime) override;

		void ManageDistanceToTarget();

		//Access the ships target
		Ship* GetTarget() const { return m_target; };
		void SetTarget(Ship& _target) { m_target = &_target; };

		float GetMaxDistance() const { return m_maxDistance; }
		void SetMaxDistance(const float _distance) { m_maxDistance = _distance; };

		float GetMinDistance() const { return m_minDistance; }
		void SetMinDistance(const float _distance) { m_minDistance = _distance; };
	private:
		Ship* m_target;
		float m_maxDistance;
		float m_minDistance;
		bool m_lookForTarget;
	};
}