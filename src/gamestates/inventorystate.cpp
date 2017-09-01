#include <unordered_map>
#include "inventorystate.hpp"
#include "mainstate.hpp"
#include "gamestate.hpp"
#include "graphic/interface/hud.hpp"
#include "control/input.hpp"
#include "gameplay/elements/ship.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "graphic/interface/hudelements.hpp"
#include "gameplay/elements/shipsystems/weapon.hpp"
#include "math/extensions.hpp"
#include "utils/stringutils.hpp"
#include "GLFW/glfw3.h"

namespace GameStates
{
	using namespace Graphic;
	using namespace Control;

	const ei::Vec2 SHIP_VIEW_SHIFT = ei::Vec2(0.f, 0.f);
	const ei::Vec2 SHIP_VIEW_SIZE = ei::Vec2(0.9f, 0.5f);

	const float UPGRADES_BASE_VALUE[Upgrades::COUNT] = { 10.0f, 1.5f, 32.0f, 10.0f, 2.0f, 132.0f };

	InventoryState::InventoryState(Game::Ship& _ship)
		: m_ship(_ship),
		m_oldCamera(Control::g_camera),
		m_money(_ship.GetInventory().GetCredits()),
		m_upgradeLvls(_ship.GetUpgradeLevels())
	{
		using namespace Game;
		using namespace ei;
		using namespace Utils;

		for (int i = 0; i < Upgrades::COUNT; i++)
			m_hud.m_upgradeBtns[i]->SetOnMouseUp([=]() { UpgradeValue((Upgrades)i); });

		// position the upgrade buttons
		UpdateUpgradeLabels();

		// put camera above the ship
		const float radius = _ship.GetGeometryComponent().GetMesh().GetMeshBounds().boundingRadius * 1.1f;
		float height = radius / tan(g_camera.GetFov() * 0.5f);
		g_camera.SetPosition(m_ship.GetPosition() + m_ship.GetRotationMatrix() * Vec3(0.f, height, 0.f));
		// look down so that the center of the ship is in the center of the camera
		g_camera.SetRotation(Quaternion(m_ship.GetRotationMatrix() * Vec3(0.f, 0.f, 1.f),
			normalize(m_ship.GetPosition() - g_camera.GetPosition())) * m_ship.GetRotation());
		g_camera.FixRotation(g_camera.GetRotation(), g_camera.GetPosition());
		g_camera.Process(0.f);

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
		ei::Mat4x4 transform = g_camera.GetViewProjection() * m_ship.GetTransformation();
		for (auto& socket : _ship.GetWeaponSockets())
		{
			// project socket coordinates to screen rectangle
			const float r = 1.f / radius;
			// model space -> ndc
			ei::Vec4 pos = transform * Vec4(socket.GetPosition(), 1.f);//socket.GetPosition() * r;
			pos /= pos.w;
			auto& socketField = m_hud.CreateScreenElement<DropField>("box_uncut", Vec2(pos.x, pos.y), PixelOffset(84, 84), DefP::MidMid, ScreenPosition::Anchor(),
				[&, i](DropField& _this, DraggableTexture& _tex)
			{
				// if the field already contains an element put that one back to the main inventory
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

		m_hud.m_sellField->SetDropEvent([&](DropField& _this, DraggableTexture& _texture) 
		{
			Game::Weapon* itm = const_cast<Game::Weapon*>(static_cast<const Game::Weapon*>(_texture.GetContent()));
			m_money += itm->GetValue();
			m_ship.GetInventory().Remove(*itm);
			itm->Destroy();
			_this.DetachElement(_texture);
			// since the inventory hud is rebuild every time they do not need to be actually destroyed here
			_texture.SetActive(false);
			_texture.SetVisible(false);
			// money total changed
			UpdateUpgradeLabels();
		});
	}

	InventoryState::~InventoryState()
	{
		const int OTHER_FIELDS = 2;
		// update changed weapons
		// the first field is the inventory, second the sell field
		for (size_t i = OTHER_FIELDS; i < m_hud.m_weaponFields.size(); ++i)
		{
			auto& elements = m_hud.m_weaponFields[i]->GetElements();
			if (elements.size())
			{
				const Game::Weapon* itm = static_cast<const Game::Weapon*>(elements.front()->GetContent());
				// the inventory does not change the weapon's state; but here full access is required
				m_ship.SetWeapon(int(i - OTHER_FIELDS), const_cast<Game::Weapon*>(itm));
			}
			else m_ship.SetWeapon(int(i - OTHER_FIELDS), nullptr);
		}
		m_ship.GetInventory().SetCredits(m_money);

		// restore camera state
		Control::g_camera = m_oldCamera;
	}

	void InventoryState::OnActivate()
	{
		// update camera again because the ship position might have been updated once
		const float radius = m_ship.GetGeometryComponent().GetMesh().GetMeshBounds().boundingRadius * 1.1f;
		float height = radius / tan(g_camera.GetFov() * 0.5f);
		g_camera.SetPosition(m_ship.GetPosition() + m_ship.GetRotationMatrix() * Vec3(0.f, height, 0.f));
		// look down so that the center of the ship is in the center of the camera
		g_camera.SetRotation(Quaternion(m_ship.GetRotationMatrix() * Vec3(0.f, 0.f, 1.f),
			normalize(m_ship.GetPosition() - g_camera.GetPosition())) * m_ship.GetRotation());
		g_camera.FixRotation(g_camera.GetRotation(), g_camera.GetPosition());
		g_camera.Process(0.f);
	}

	// ******************************************************** //
	void InventoryState::UpdateUpgradeLabels()
	{
		using namespace Utils;
		using namespace Game;

		// show basic stats
		m_hud.m_shipInfoLabel->SetText("ship properties\n"s + '\n'
			+ "energy:   " + ToConstDigit(m_ship.GetEnergy(), 1, 5) + " / " + ToConstDigit(m_ship.GetMaxEnergy(), 1, 5) + '\n'
			+ "recharge: " + ToConstDigit(m_ship.GetEnergyRecharge(), 1, 13) + "\n\n"
			+ "shield:   " + ToConstDigit(m_ship.GetShield(), 1, 5) + " / " + ToConstDigit(m_ship.GetMaxShield(), 1, 5) + '\n'
			+ "recharge: " + ToConstDigit(m_ship.GetShieldRecharge(), 1, 13) + "\n"
			+ "delay:    " + ToConstDigit(m_ship.GetShieldDelay(), 1, 13) + "\n\n"
			+ "hull:     " + ToConstDigit(m_ship.GetHealth(), 1, 5) + " / " + ToConstDigit(m_ship.GetMaxHealth(), 1, 5) + "\n\n");

		Vec2 margin = PixelOffset(10.0f, 0.0f);
		float values[Upgrades::COUNT]{
			m_ship.GetMaxEnergy(),
			m_ship.GetEnergyRecharge(),
			m_ship.GetMaxShield(),
			m_ship.GetShieldRecharge(),
			m_ship.GetShieldDelay(),
			m_ship.GetMaxHealth()
		};
		for (int i = 0; i < Upgrades::COUNT; i++)
		{
			if (GetUpgradeCost((Upgrades)i) <= m_money)
			{
				if (i != Upgrades::SHIELD_REG_DELAY)
					m_hud.m_upgradeLabels[i]->SetText("(+" + ToConstDigit(NextUpgradeValue((Upgrades)i) - values[i], 1, 5) + ")");
				else
					m_hud.m_upgradeLabels[i]->SetText("(" + ToConstDigit(NextUpgradeValue((Upgrades)i) - values[i], 1, 6) + ")");
				m_hud.m_upgradeBtns[i]->SetVisible(true);
			}
			else
			{
				m_hud.m_upgradeLabels[i]->SetText("");
				m_hud.m_upgradeBtns[i]->SetVisible(false);
			}
			m_hud.m_upgradeLabels[i]->SetPosition(margin + m_hud.m_shipInfoLabel->GetPosition() + Vec2(m_hud.m_shipInfoLabel->GetRectangle().x, -m_rows[i] * m_hud.m_shipInfoLabel->GetCharSize().y * m_hud.m_shipInfoLabel->GetDefaultSize()));
			m_hud.m_upgradeBtns[i]->SetPosition(margin + m_hud.m_upgradeLabels[i]->GetPosition() + Vec2(m_hud.m_upgradeLabels[i]->GetRectangle().x, 0.0f));
		}

		m_hud.m_moneyLabel->SetPosition(m_hud.m_shipInfoLabel->GetPosition() + Vec2(0.0f, -m_hud.m_shipInfoLabel->GetRectangle().y));
	}

	int InventoryState::GetUpgradeCost(Upgrades _upgrade)
	{
		return 100 + m_upgradeLvls[_upgrade] * 50;
		/*switch (_upgrade)
		{
		case Upgrades::ENERGY:
			return m_upgradeLvls[_upgrade];
			break;
		case Upgrades::ENERGY_REG:
			return 2;
			break;
		case Upgrades::SHIELD:
			return 3;
			break;
		case Upgrades::SHIELD_REG:
			return 4;
			break;
		case Upgrades::SHIELD_REG_DELAY:
			return 5;
			break;
		case Upgrades::HULL:
			return 6;
			break;
		}*/
	}

	float InventoryState::NextUpgradeValue(Upgrades _upgrade)
	{
		if (_upgrade != Upgrades::SHIELD_REG_DELAY)
			return UPGRADES_BASE_VALUE[_upgrade] + (m_upgradeLvls[_upgrade] + 1) * 0.1f * UPGRADES_BASE_VALUE[_upgrade];
		else
			return UPGRADES_BASE_VALUE[_upgrade] - (m_upgradeLvls[_upgrade] + 1) * 0.1f * UPGRADES_BASE_VALUE[_upgrade];
	}

	void InventoryState::UpgradeValue(Upgrades _upgrade)
	{
		if (m_money >= GetUpgradeCost(_upgrade))
		{
			m_money -= GetUpgradeCost(_upgrade);
			float newVal = NextUpgradeValue(_upgrade);
			m_upgradeLvls[_upgrade]++;
			switch (_upgrade)
			{
			case Upgrades::ENERGY:
				m_ship.SetMaxEnergy(newVal);
				break;
			case Upgrades::ENERGY_REG:
				m_ship.SetEnergyRecharge(newVal);
				break;
			case Upgrades::SHIELD:
				m_ship.SetMaxShield(newVal);
				break;
			case Upgrades::SHIELD_REG:
				m_ship.SetShieldRecharge(newVal);
				break;
			case Upgrades::SHIELD_REG_DELAY:
				m_ship.SetShieldDelay(newVal);
				break;
			case Upgrades::HULL:
				m_ship.SetMaxHealth(newVal);
				break;
			}
			UpdateUpgradeLabels();
		}
	}

	void InventoryState::Process(float _deltaTime)
	{
		using namespace Utils;

		std::string text = "money:    " + ToConstDigit(m_money, 0, 6) + " $";
		for (int i = 0; i < Upgrades::COUNT; i++)
		{
			if (m_hud.m_upgradeBtns[i]->GetButtonState() == Graphic::Button::State::MouseOver && m_hud.m_upgradeBtns[i]->IsVisible())
				text += " (-" + ToConstDigit(GetUpgradeCost((Upgrades)i), 0, 6) + "$)";
		}
		m_hud.m_moneyLabel->SetText(text);
		//if (InputManager::IsVirtualKeyPressed(Control::VirtualKey::INVENTORY))
		//{
		//	// Change back to main state
		//	m_newState = new GameStates::MainState();
		//}
	}

	void InventoryState::Draw(float _deltaTime)
	{
		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		m_ship.GetGeometryComponent().Draw();

		Device::DrawFramebufferToBackbuffer();

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
		if (InputManager::IsVirtualKey(_key, Control::VirtualKey::INVENTORY) || _key == GLFW_KEY_ESCAPE) m_isFinished = true;
		if (m_hud.KeyUp(_key, 0)) return;
	}
	void InventoryState::KeyClick(int _key)
	{

	}
	void InventoryState::KeyDoubleClick(int _key)
	{

	}
}