#pragma once

#include "ei/vector.hpp"
#include <vector>
#include <memory>

namespace Game {

	enum struct DamageType
	{
		Normal,
		Pure
	};

	/* Actor *************************************
	 * Basic class for objects that exist in the world.
	 */

	class Actor
	{
	public:
		Actor();
		Actor(const ei::Vec3& _position, const ei::Quaternion& _rotation = ei::qidentity());
		Actor(const Actor& _orig);
		// this could be implemented sensibly but the auto generated version does not work due to m_handle
		Actor(Actor&&) = delete;
		Actor& operator=(const Actor&) = delete;
		Actor& operator=(Actor&&) = delete;

		virtual ~Actor();

		// Override this to register components.
		virtual void RegisterComponents(class SceneGraph& _sceneGraph) {}

		// Access Position
		void SetPosition(const ei::Vec3& _position) { m_position = _position; UpdateMatrices(); }
		const ei::Vec3& GetPosition() const { return m_position; }
		void Translate(const ei::Vec3& _position) { m_position += _position; }

		// Rotate around the given rotation.
		void SetRotation(const ei::Quaternion& _rotation) { m_rotation = _rotation; UpdateMatrices();}
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

		float GetHealth() const { return m_health; }
		void SetHealth(float _health) { m_health = _health; }
		float GetMaxHealth() const { return m_maxHealth; }

		// Destroys the object
		void Destroy(bool _callOnDestroy = true);
		// Is object to be destroyed?
		bool IsDestroyed() const { return m_isDestroyed; }
		void Damage(float _amount, Actor& _source, DamageType _type = DamageType::Normal);

		virtual void OnDestroy() {}
		// Collision with another Actor was registered.
		virtual void OnCollision(Actor& _other) {}
		// Decide how damage is handled.
		// Overwrite to implement custom behavior such as reduction or absorption.
		virtual float OnDamageTaken(float _amount, Actor& _source, DamageType _type) { return _amount; }
		virtual void Process(float _deltaTime) {}

		bool CanTick() const { return m_canTick; }

		// Since Actors can be destroyed a persistent reference to an Actor
		// works like a weak_ptr.
		// A Handle can have multiple states.
		// When not coming directly from an Actor the handle itself might be invalid:
		// if(handle) cout << "is valid";
		// After an Actor has been destroyed all valid handles point on a nullptr:
		// if(*handle) cout << "The Actor still exists.";
		// Then the Actor can be accessed:
		// Actor& act = **m_handle;
		struct HandleImpl
		{
			HandleImpl(Actor& _actor) : actor(&_actor) {}

			Actor& operator*()
			{
				return *actor;
			}

			Actor* operator->()
			{
				return actor;
			}

			operator bool() { return actor != nullptr; }
		private:
			friend class Actor;
			Actor* actor;
		};
		typedef std::shared_ptr<HandleImpl> Handle;

		Handle GetHandle() { return m_handle; }
		const Handle GetHandle() const { return m_handle; }
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
		float m_maxHealth;

		Handle m_handle;
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
		void SetInertiaTensor(const ei::Mat3x3& _tensor) { m_inertiaTensor = _tensor; m_inverseInertiaTensor = ei::invert(_tensor); }
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