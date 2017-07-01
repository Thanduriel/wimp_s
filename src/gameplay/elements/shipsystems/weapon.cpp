#include "weapon.hpp"
#include "gameplay/scenegraph.hpp"
#include "projectile.hpp"

namespace Game {

	Weapon::Weapon(const ei::Vec3& _position, float _cooldown)
		: Actor(_position),
		m_factoryComponent(THISACTOR),
		m_cooldown(0.f),
		m_cooldownMax(_cooldown)
	{

	}

	void Weapon::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_factoryComponent);
	}

	void Weapon::Process(float _deltaTime)
	{
		Actor::Process(_deltaTime);

		m_cooldown -= _deltaTime;
	}

	void Weapon::Fire()
	{
		if (m_cooldown < 0.f)
		{
			m_cooldown = m_cooldownMax;
			m_factoryComponent.MakeP<Rocket>(ei::Vec3(0.f), m_rotationMatrix * ei::Vec3(0.f, 0.f, 12.f), 4.f);
		}
	}
}