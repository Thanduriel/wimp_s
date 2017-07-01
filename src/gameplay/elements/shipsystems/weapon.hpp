#pragma once

#include "gameplay/core/actor.hpp"
#include "gameplay/elements/factorycomponent.hpp"

namespace Game {

	class Weapon : public Actor
	{
	public:
		Weapon(const ei::Vec3& _position = ei::Vec3(), float _cooldown = 1.f);

		void RegisterComponents(class SceneGraph& _sceneGraph) override;

		void Process(float _deltaTime) override;
		// Fires this weapon if it is ready.
		void Fire();

		float GetEnergyCost() const { return m_energyCost; }
	protected:
		float m_cooldown;
		float m_cooldownMax;

		float m_energyCost;
	private:
		FactoryComponent m_factoryComponent;
	};
}