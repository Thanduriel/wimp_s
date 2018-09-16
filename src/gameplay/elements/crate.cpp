#include "crate.hpp"
#include "gameplay/scenegraph.hpp"
#include "gameplay/elements/ship.hpp"
#include "generators/random.hpp"

namespace Game {

	static thread_local Generators::RandomGenerator rng(0x3161f);

	Crate::Crate(const ei::Vec3& _position, Inventory& _inventory, float _pickupRadius)
		: DynamicActor(_position, rng.Rotation()),
		m_pickUp(THISACTOR, _pickupRadius),
		m_visual(THISACTOR,"crate" )
	{
		m_canTakeDamage = false;
		m_inventory.Transfer(_inventory);
	}

	void Crate::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_pickUp);
		_sceneGraph.RegisterComponent(m_visual);
	}

	void Crate::PickUpEvent::operator()(Actor& _self, Actor& _other)
	{
		Crate& crate = static_cast<Crate&>(_self);
		Ship& ship = static_cast<Ship&>(_other);
		ship.GetInventory().Transfer(crate.m_inventory);
		crate.Destroy();
	}
}