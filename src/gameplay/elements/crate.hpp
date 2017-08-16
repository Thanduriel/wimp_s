#pragma once

#include "gameplay/core/actor.hpp"
#include "gameplay/core/model.hpp"
#include "gameplay/core/component.hpp"
#include "shipsystems/inventory.hpp"
#include "collisioncomponent.hpp"
#include "triggervolumecomponent.hpp"

namespace Game {

	/* Crate ****************************************
	 * A container for items that can be placed in space and picked up
	 * by getting close to it.
	 */
	class Crate : public Actor
	{
	public:
		// Create a crate at _position that takes all items form _inventory
		Crate(const ei::Vec3& _position, Inventory& _inventory, float _pickupRadius = 50.f);

		void RegisterComponents(class SceneGraph& _sceneGraph) override;
	private:
		struct PickUpEvent
		{
			void operator()(Actor& _self, Actor& _other);
		};
		TriggerVolumeComponent<PickUpEvent> m_pickUp;
		GeometryComponent m_visual;
		Inventory m_inventory;
	};
}