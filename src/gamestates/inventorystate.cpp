#include <unordered_map>
#include "inventorystate.hpp"
#include "mainstate.hpp"
#include "gamestate.hpp"
#include "huds/inventoryhud.hpp"
#include "graphic/interface/hud.hpp"
#include "control/input.hpp"
#include "gameplay/elements/ship.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "graphic/interface/hudelements.hpp"
#include "gameplay/elements/shipsystems/weapon.hpp"

namespace GameStates
{
	using namespace Graphic;
	using namespace Control;

	const ei::Vec2 SHIP_VIEW_SHIFT = ei::Vec2(0.f, 0.f);
	const ei::Vec2 SHIP_VIEW_SIZE = ei::Vec2(0.9f, 0.5f);

	InventoryState::InventoryState(Game::Ship& _ship)
		: m_ship(_ship)
	{
		using namespace Game;

		// remember item -> texture relation to find them when handling equipped weapons
		std::unordered_map<const Item*, DraggableTexture*> itemIcons;
		for (const Item* item : _ship.GetInventory())
		{
			// since Weapon uses multi-inheritance Item has to be casted first to get the actual weapon pointer
			auto& tex = m_hud.CreateScreenElement<DraggableTexture>("weaponIcon", Vec2(0.f), PixelOffset(64, 64), DefP::MidMid, ScreenPosition::Anchor(DefP::MidMid, &m_hud), m_hud.m_weaponFields, static_cast<const Weapon*>(item));
			tex.SetColor(Item::QUALITY_COLOR[(int)item->GetQuality()]);
			m_hud.m_inventoryField->DropElement(tex);

			itemIcons.emplace(item, &tex);
		}

		int i = 0;
		for (auto& socket : _ship.GetWeaponSockets())
		{
			// project socket coordinates to screen rectangle
			const float r = 1.f / _ship.GetGeometryComponent().GetMesh().GetMeshBounds().boundingRadius;
			ei::Vec3 pos = socket.GetPosition() * r;
			auto& socketField = m_hud.CreateScreenElement<DropField>("box_uncut", Vec2(pos.x, pos.z) * SHIP_VIEW_SIZE + SHIP_VIEW_SHIFT, PixelOffset(84, 84), DefP::MidMid, ScreenPosition::Anchor(),
				[&,i](DropField& _this, DraggableTexture& _tex) 
			{
				// if the field alraidy contains an element put that one back to the main inventory
				if (_this.GetElements().size())
				{
					DraggableTexture& tex = *_this.GetElements().back();
					m_hud.m_inventoryField->DropElement(tex);
					tex.SetPosition(tex.GetBackupPosition());
				}
			});
			m_hud.m_weaponFields.push_back(&socketField);

			if (const Weapon* weapon = static_cast<const Weapon*>(socket.GetAttached()))
			{
				auto it = itemIcons.find(static_cast<const Item*>(weapon));
				Assert(it != itemIcons.end(), "The equipped weapon is not found in the inventory.");
				socketField.DropElement(*it->second);

				// since the icons are created first the sockets are in front of them
				m_hud.MoveToFront(*it->second);
			}
			++i;
		}
	}

	InventoryState::~InventoryState()
	{
		// update changed weapons
		// the first field is the inventory
		for (size_t i = 1; i < m_hud.m_weaponFields.size(); ++i)
		{
			auto& elements = m_hud.m_weaponFields[i]->GetElements();
			if (elements.size())
			{
				const Game::Weapon* itm = static_cast<const Game::Weapon*>(elements.front()->GetContent());
				// the inventory does not change the weapon's state; but here full access is required
				m_ship.SetWeapon(int(i-1), const_cast<Game::Weapon*>(itm));
			}
			else m_ship.SetWeapon(int(i - 1), nullptr);
		}
	}

	void InventoryState::Process(float _deltaTime)
	{
		//if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::INVENTORY))
		//{
		//	// Change back to main state
		//	m_newState = new GameStates::MainState();
		//}
	}

	void InventoryState::Draw(float _deltaTime)
	{
		Texture& tex = *Device::GetCurrentFramebufferBinding()->GetColorAttachments().begin()->pTexture;
		Device::BindFramebuffer(nullptr);
		Device::SetEffect(Resources::GetEffect(Effects::SCREEN_OUTPUT));
		Device::SetTexture(tex, 0);
		Device::DrawFullscreen();
		m_hud.Draw(_deltaTime);
	}

	void InventoryState::Dispose()
	{
	}

	// ******************************************************* //
	void InventoryState::MouseMove(float _dx, float _dy)
	{
		m_hud.MouseMove(_dx, _dy);
	}
	void InventoryState::Scroll(float _dx, float _dy)
	{
		if (m_hud.Scroll(_dx, _dy)) return;
	}
	void InventoryState::KeyDown(int _key, int _modifiers)
	{
		if (m_hud.KeyDown(_key, _modifiers)) return;
	}
	void InventoryState::KeyRelease(int _key)
	{
		if (InputManager::IsVirtualKey(_key, Control::VirtualKey::INVENTORY)) m_isFinished = true;
		if (m_hud.KeyUp(_key, 0)) return;
	}
	void InventoryState::KeyClick(int _key)
	{

	}
	void InventoryState::KeyDoubleClick(int _key)
	{

	}
}