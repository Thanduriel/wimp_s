#include "actor.hpp"
#include "component.hpp"

namespace Game {

	using namespace ei;

	Actor::Actor()
		: m_position(0.f), m_rotation(ei::qidentity()), m_isDestroyed(false)
	{
		UpdateMatrices();
	}

	Actor::Actor(const ei::Vec3& _position, const ei::Quaternion& _rotation)
		: m_position(_position),
		m_rotation(_rotation),
		m_isDestroyed(false),
		m_canTakeDamage(false),
		m_health(0.f)
	{
		UpdateMatrices();
	}

	Actor::~Actor()
	{
	}

	void Actor::UpdateMatrices()
	{
		m_rotationMatrix = rotation(m_rotation);
		m_inverseRotationMatrix = transpose(m_rotationMatrix);

		m_transformation = translation(m_position) * Mat4x4(m_rotationMatrix);
		m_inverseTransformation = Mat4x4(m_inverseRotationMatrix) * translation(-m_position);
	}

	void Actor::Destroy()
	{
		m_isDestroyed = true;
		OnDestroy();
	}

	void Actor::Damage(float _amount, Actor& _source)
	{
		float amount = OnDamageTaken(_amount, _source);
		if (m_canTakeDamage)
		{
			m_health -= amount;
			if (m_health < 0.f) Destroy();
		}
	}

	void Actor::RotateAroundLocal(const Vec3& axis, const float degrees)
	{
		Vec3 localToGlobal = ei::rotation(GetRotation()) * axis;
		localToGlobal *= degrees;
		Rotate(Quaternion(localToGlobal.x, localToGlobal.y, localToGlobal.z));
	}


	// ****************************************************************** //
	DynamicActor::DynamicActor(const ei::Vec3& _position, const ei::Quaternion& _rotation)
		: Actor(_position, _rotation),
		m_velocity(0.f),
		m_angularVelocity(0.f),
		m_mass(0.f),
		m_inertiaTensor(ei::identity3x3()),
		m_inverseInertiaTensor(ei::identity3x3())
	{}

	void DynamicActor::Process(float _deltaTime)
	{
		m_position += m_velocity * _deltaTime;

		// Create a delta Quaternion from rotation velocity
		// http://physicsforgames.blogspot.de/2010/02/quaternions.html
		Quaternion deltaRot;
		Vec3 theta = m_angularVelocity * _deltaTime;
		float thetaMagSq = lensq(theta);
		float s;
		if (thetaMagSq * thetaMagSq / 24.0f < 1e-6f)
		{
			deltaRot.r = 1.0f - thetaMagSq / 2.0f;
			s = 1.0f - thetaMagSq / 6.0f;
		}
		else
		{
			float thetaMag = sqrt(thetaMagSq);
			deltaRot.r = cos(thetaMag);
			s = sin(thetaMag) / thetaMag;
		}
		deltaRot.i = theta[0] * s;
		deltaRot.j = theta[1] * s;
		deltaRot.k = theta[2] * s;

		Rotate(deltaRot);

		UpdateMatrices();
	}
}