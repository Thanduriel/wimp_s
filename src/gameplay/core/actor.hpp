#pragma once

#include "ei/vector.hpp"

namespace Game {

	/* Actor *************************************
	 * Basic class for objects that exist in the world.
	 */

	class Actor
	{
	public:
		Actor() : m_position(0.f), m_rotation(ei::qidentity()), m_destroyed(false) { UpdateMatrices(); }
		Actor(const ei::Vec3& _position, const ei::Quaternion& _rotation);
		virtual ~Actor() {}

		// Access Position
		void SetPosition(const ei::Vec3& _position) { m_position = _position; UpdateMatrices(); }
		const ei::Vec3& GetPosition() const { return m_position; }
		void Translate(const ei::Vec3& _position) { m_position += _position; }

		// Rotate around the given rotation.
		void SetRotation(const ei::Quaternion& _rotation) { m_rotation = _rotation; }
		const ei::Quaternion& GetRotation() const { return m_rotation; }
		void Rotate(const ei::Quaternion& _rotation) { m_rotation = _rotation * m_rotation; UpdateMatrices(); }

		// This matrix transforms modelspace -> worldspace
		const ei::Mat4x4& GetTransformation() const { return m_transformation; }

		// Destroys the object
		void Destroy();
		// Is object to be destroyed?
		bool IsDestroyed() { return m_destroyed; }

		virtual void Process(float _deltaTime) {};
		virtual void Draw() {};

		bool CanTick() const { return m_canTick; }
	protected:
		void UpdateMatrices();

		bool m_canTick;

		ei::Vec3 m_position;
		ei::Quaternion m_rotation;

		// transformation matrices for convinience
		ei::Mat3x3 m_rotationMatrix;
		ei::Mat3x3 m_inverseRotationMatrix;
		ei::Mat4x4 m_transformation;

		// shows if the object is to be destroyed
		bool m_destroyed;
	};


	/* DynamicActor ***********************************
	 * An actor that can have momentum.
	 */
	class DynamicActor : public Actor
	{
	public:
		DynamicActor(const ei::Vec3& _position, const ei::Quaternion& _rotation);

		// Access Velocity
		void SetVelocity(const ei::Vec3& _velocity) { m_velocity = _velocity; }
		const ei::Vec3& GetVelocity() const { return m_velocity; }

		void SetAngularVelocity(const ei::Vec3& _angVel) { m_angularVelocity = _angVel; }
		const ei::Vec3& GetAngularVelocity() const { return m_angularVelocity; }

		// Access weight
		void SetWeight(const float _weight) { m_weight = _weight; }
		float GetWeight() const { return m_weight; }

		void Process(float _deltaTime) override;
	private:
		ei::Vec3 m_velocity;
		ei::Vec3 m_angularVelocity; // the length is the rotation speed
		float m_weight;
	};
}