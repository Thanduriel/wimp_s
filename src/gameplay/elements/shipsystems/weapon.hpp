#pragma once

#include "gameplay/core/actor.hpp"
#include "gameplay/elements/factorycomponent.hpp"

namespace Game {

	class Weapon : public Actor
	{
	public:
		Weapon(const ei::Vec3& _position = ei::Vec3());

		void RegisterComponents(class SceneGraph& _sceneGraph) override;

		void Fire();

	private:
		FactoryComponent m_factoryComponent;
	};
}