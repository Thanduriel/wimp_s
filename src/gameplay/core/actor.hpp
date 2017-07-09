#pragma once

#include "ei/vector.hpp"
#include <vector>
#include <memory>

namespace Game {

	/* Actor *************************************
	 * Basic class for objects that exist in the world.
	 */

	class Actor
	{
	public:
		Actor();
		Actor(const ei::Vec3& _position, const ei::Quaternion& _rotation = ei::qidentity());
		virtual ~Actor();

		// Override this to register components.
		virtual void RegisterComponents(class SceneGraph& _sceneGraph) {}

		// Access Position
		void SetPosition(const ei::Vec3& _position) { m_position = _position; UpdateMatrices(); }
		const ei::Vec3& GetPosition() const { return m_position; }
		void Translate(const ei::Vec3& _position) { m_position += _position; }

		// Rotate around the given rotation.
		void SetRotation(const ei::Quaternion& _rotation) { m_rotation = _rotation; }
		const ei::Quaternion& GetRotation() const { return m_rotation; }
		void Rotate(const ei::Quaternion& _rotation) { m_rotation = _rotation * m_rotation; UpdateMatrices(); }

		//Some local rotation functions
		void RotateAroundLocal(const ei::Vec3& axis, const float degrees);
		//Around local x
		void Pitch(const float degrees) { RotateAroundLocal(ei::Vec3(1.0f, 0.0f, 0.0f), degrees); };
		//Around local y
		void Yaw(const float degrees) { RotateAroundLocal(ei::Vec3(0.0f, 1.0f, 0.0f), degrees); };
		//Around local z
		void Roll(const float degrees) { RotateAroundLocal(ei::Vec3(0.0f, 0.0f, 1.0f), degrees); };

		// This matrix transforms modelspace -> worldspace
		const ei::Mat4x4& GetTransformation() const { return m_transformation; }
		// worldspace -> modelspace
		const ei::Mat4x4& GetInverseTransformation() const { return m_inverseTransformation; }
		const ei::Mat3x3& GetRotationMatrix() const { return m_rotationMatrix; }
		const ei::Mat3x3& GetInverseRotationMatrix() const { return m_inverseRotationMatrix; }

		// Destroys the object
		void Destroy();
		// Is object to be destroyed?
		bool IsDestroyed() const { return m_isDestroyed; }
		void Damage(float _amount, Actor& _source);

		virtual void OnDestroy() {}
		// Collision with another Actor was registered.
		virtual void OnCollision(Actor& _other) {}
		// Decide how damage is handled.
		// Overwrite to implement custom behavior such as reduction or absorption.
		virtual float OnDamageTaken(float _amount, Actor& _source) { return _amount; }
		virtual void Process(float _deltaTime) {}

		bool CanTick() const { return m_canTick; }
	protected:
		void UpdateMatrices();

		bool m_canTick;
		bool m_canTakeDamage;

		ei::Vec3 m_position;
		ei::Quaternion m_rotation;

		// transformation matrices for convenience
		ei::Mat3x3 m_rotationMatrix;
		ei::Mat3x3 m_inverseRotationMatrix;
		ei::Mat4x4 m_transformation;
		ei::Mat4x4 m_inverseTransformation;

		// shows if the object is to be destroyed
		bool m_isDestroyed;
		float m_health;
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
		void AddVelocity(const ei::Vec3& _velocity) { m_velocity += _velocity; }
		const ei::Vec3& GetVelocity() const { return m_velocity; }

		void SetAngularVelocity(const ei::Vec3& _angVel) { m_angularVelocity = _angVel; }
		void AddAngularVelocity(const ei::Vec3& _angVel) { m_angularVelocity += _angVel; }
		const ei::Vec3& GetAngularVelocity() const { return m_angularVelocity; }

		const ei::Mat3x3& GetInertiaTensor() const { return m_inertiaTensor; }
		const ei::Mat3x3& GetInverseInertiaTensor() const { return m_inverseInertiaTensor; }
		float GetMass() const { return m_mass; }

		void Process(float _deltaTime) override;
	protected:
		float m_mass; // mass in [kg]
		ei::Mat3x3 m_inertiaTensor;
		ei::Mat3x3 m_inverseInertiaTensor;

		ei::Vec3 m_velocity;
		ei::Vec3 m_angularVelocity; // the length is the rotation speed
	};
}