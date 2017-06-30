#include "weapon.hpp"
#include "gameplay/scenegraph.hpp"
#include "projectile.hpp"

namespace Game {

	Weapon::Weapon(const ei::Vec3& _position)
		: Actor(_position),
		m_factoryComponent(THISACTOR)
	{

	}

	void Weapon::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_factoryComponent);
	}

	void Weapon::Fire()
	{
		m_factoryComponent.Make<Rocket>(ei::Vec3(0.f), m_rotationMatrix * ei::Vec3(0.f,0.f,10.f), 10.f);
	}
}