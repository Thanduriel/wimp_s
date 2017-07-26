#include "socketcomponent.hpp"

namespace Game {

	SocketComponent::SocketComponent(Actor& _owner, const ei::Vec3& _position)
		: ConstActorComponent(_owner),
		Transformation(_position),
		m_target(nullptr)
	{

	}

	void SocketComponent::ProcessComponent(float _deltaTime)
	{
		if (m_target)
		{
			ei::Mat4x4 transform = Transformation::Get(m_actor.GetTransformation());
			m_target->SetPosition(ei::Vec3(transform * ei::Vec4(0.f, 0.f, 0.f, 1.f)));

			// stabilize numeric
			ei::Mat3x3 rot = ei::Mat3x3(transform);
			ei::orthonormalize(rot);
			m_target->SetRotation(ei::Quaternion(rot));
		}
	}
}