#include "crate.hpp"
#include "gameplay/scenegraph.hpp"
#include "gameplay/elements/ship.hpp"
#include "generators/random.hpp"
#include "gameplay/content.hpp"
#include "gameplay/elements/audiocomponent.hpp"
#include "gameplay/elements/shipsystems/item.hpp"
#include "gameplay/elements/shipsystems/weapon.hpp"
#include "gameplay/elements/shipsystems/shield.hpp"

namespace Game {
	using namespace Generators;

	Crate::Crate(const ei::Vec3& _position, Inventory& _inventory, float _pickupRadius)
		: DynamicActor(_position, g_random.Rotation()),
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
		static const auto& pickUpSound = Content::GetSound("pickup");
		AudioSystem::GetGlobalAudio().Play(pickUpSound);
		ship.GetInventory().Transfer(crate.m_inventory);
		crate.Destroy();
	}
}