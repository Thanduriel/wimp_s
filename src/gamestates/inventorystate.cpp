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
	{
		using namespace Game;
		int i = 0;
		for (auto& socket : _ship.GetWeaponSockets())
		{
			const float r = 1.f / _ship.GetGeometryComponent().GetMesh().GetMeshBounds().boundingRadius;
			ei::Vec3 pos = socket.GetPosition() * r;
			auto& socketField = m_hud.CreateScreenElement<DropField>("box_uncut", Vec2(pos.x, pos.z) * SHIP_VIEW_SIZE + SHIP_VIEW_SHIFT, PixelOffset(84, 84), DefP::MidMid, ScreenPosition::Anchor(),
				[&_ship,i](DraggableTexture& _tex) 
			{
				const Game::Weapon* itm = static_cast<const Game::Weapon*>(_tex.GetContent());
				// the inventory does not change the weapon's state; but here full access is required
				_ship.SetWeapon(i, const_cast<Game::Weapon*>(itm));
			});
			m_hud.m_weaponFields.push_back(&socketField);

			if (const Weapon* weapon = static_cast<const Weapon*>(socket.GetAttached()))
			{
				auto& tex = m_hud.CreateScreenElement<DraggableTexture>("box_uncut", Vec2(0.f), PixelOffset(64, 64), DefP::MidMid, ScreenPosition::Anchor(DefP::MidMid, &m_hud), m_hud.m_weaponFields, weapon);
				socketField.DropElement(tex);
				tex.SetPosition(tex.GetBackupPosition());
			}
			++i;
		}
	}

	InventoryState::~InventoryState()
	{
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