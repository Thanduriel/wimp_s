#pragma once

#include "gameplay/core/component.hpp"
#include "math/transformation.hpp"

namespace Game {

	class SocketComponent : public ConstActorComponent, public Transformation
	{
	public:
		SocketComponent(Actor& _owner, const ei::Vec3& _position);

		void Attach(Actor& _target) { m_target = &_target; }
		void Detach() { m_target = nullptr; }
		Actor* GetAttached() { return m_target; }

		void ProcessComponent(float _deltaTime) override;
	private:
		Actor* m_target;
	};
}