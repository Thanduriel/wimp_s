#pragma once

#include "ei/vector.hpp"

namespace Core {

	class Actor
	{
	public:
		// Access Position
		void SetPosition(const ei::Vec3& _position) { m_position = _position; }
		const ei::Vec3& GetPosition() const { return m_position; }
		void Translate(const ei::Vec3& _position) { m_position += _position; }

		// Rotate around the given rotation.
		void Rotate(const ei::Quaternion& _rotation) { m_rotation = _rotation * m_rotation; }

		virtual void Process(float _deltaTime) {};
		virtual void Draw(float _deltaTime) {}; // might not work with an efficient renderer

		bool CanTick() const { return m_canTick; }
	protected:
		bool m_canTick;

		ei::Vec3 m_position;
		ei::Quaternion m_rotation;
	};

	class DynamicActor : public Actor
	{
	public:
		// Access Velocity
		void SetVelocity(const ei::Vec3& _velocity) { m_velocity = _velocity; }
		const ei::Vec3& GetVelocity() const { return m_velocity; }

		void SetAngularVelocity(const ei::Vec3& _angVel) { m_angularVelocity = _angVel; }
		const ei::Vec3& GetAngularVelocity() const { return m_angularVelocity; }

		void Process(float _deltaTime) override;
	private:
		ei::Vec3 m_velocity;
		ei::Vec3 m_angularVelocity;
	};
}