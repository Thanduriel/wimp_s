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
#include "gameplay/elements/shipsystems/shield.hpp"
#include "math/extensions.hpp"
#include "utils/stringutils.hpp"
#include "GLFW/glfw3.h"
#include "gameplay/elements/audiocomponent.hpp"
#include "gameplay/content.hpp"
#include "gameplay/elements/audiocomponent.hpp"

namespace GameStates
{
	using namespace Graphic;
	using namespace Control;

	// fields that are not equipment sockets
	// todo: use more elegant approach
	const int OTHER_FIELDS = 2;

	const ei::Vec2 SHIP_VIEW_SHIFT = ei::Vec2(0.f, 0.f);
	const ei::Vec2 SHIP_VIEW_SIZE = ei::Vec2(0.9f, 0.5f);

	const float UPGRADES_BASE_VALUE[Upgrades::COUNT] = { 8.0f, 1.5f, 32.0f, 6.0f, 4.0f, 80.0f };

	InventoryState::InventoryState(Game::Ship& _ship)
		: m_ship(_ship),
		m_oldCamera(Control::g_camera),
		m_money(_ship.GetInventory().GetCredits()),
		m_upgradeLvls(_ship.GetUpgradeLevels()),
		m_shouldPlaySounds(false),
		m_grid(m_ship.GetPosition() - m_ship.GetRotationMatrix() * Vec3(0.f, 100.f, 0.f),
			Utils::Color32F(0.2f, 0.1f, 0.9f, 0.3f), 3.5f, 3.5f, 130.f,
			Game::GridComponent::TransitionInfo(20000.f, 1.f, &Game::GridComponent::Random))
	{
		using namespace Game;
		using namespace ei;
		using namespace Utils;

		AudioSystem::Mute3DSounds();

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
		// build icons for every item
		for (const Item* item : _ship.GetInventory())
		{
			// since Weapon uses multi-inheritance Item has to be casted first to get the actual weapon pointer
			// do not register here so that they can be in front of the sockets
			auto& tex = m_hud.CreateScreenElementNoRegister<ItemIcon>(*item, Vec2(0.f), 
				PixelOffset(64, 64), DefP::MidMid, 
				ScreenPosition::Anchor(DefP::MidMid, &m_hud), 
				item->GetType() == Item::Type::Weapon ?  m_hud.m_weaponFields : m_hud.m_shieldFields, 
				static_cast<const Weapon*>(item));

			m_hud.m_inventoryField->DropElement(tex);

			m_itemIcons.emplace(item, &tex);
		}

		m_hud.m_inventoryField->SetDropEvent([this](DropField&, DraggableTexture& _tex)
		{
			const Game::Item* itm = static_cast<const Game::Item*>(_tex.GetContent());
			if (itm->IsEquiped())
			{
				itm->UnEquip(m_ship);
				UpdateEquipment();
				UpdateUpgradeLabels();
			}
		});

		// one drop field per item socket
		int i = 0;
		ei::Mat4x4 transform = g_camera.GetViewProjection() * m_ship.GetTransformation();
		for (auto& socket : _ship.GetWeaponSockets())
		{
			// project socket coordinates to screen rectangle
			const float r = 1.f / radius;
			// model space -> ndc
			ei::Vec4 pos = transform * Vec4(socket.GetPosition(), 1.f);//socket.GetPosition() * r;
			pos /= pos.w;
			auto& socketField = CreateItemSocket(Vec2(pos.x, pos.y), static_cast<Weapon*>(socket.GetAttached()),
				m_hud.m_weaponFields);

			// buttons to change weapon group
			auto& groupKey = m_hud.CreateScreenElement<Button>("slotBtn", Vec2(0.f), socketField.GetSize() * Vec2(0.34f, 0.34f), DefP::TopLeft,
				ScreenPosition::Anchor(DefP::BotLeft, &socketField));
			groupKey.SetOnMouseUp([i, &groupKey, &_ship]()
			{
				groupKey.SetCaption(groupKey.GetCaption() == "L" ? "R" : "L");
				// update happens on button press instead of inventory close
				_ship.SetWeaponGroup(i, groupKey.GetCaption() == "L" ? Ship::WeaponGroup::Primary
					: Ship::WeaponGroup::Secondary);
			});
			groupKey.SetCaption(_ship.GetWeaponGroup(i) == Ship::WeaponGroup::Primary ? "L" : "R");
			++i;
		}

		// shield socket
		CreateItemSocket(Vec2(0.f, 0.f), m_ship.GetEquipedShield(), m_hud.m_shieldFields);

		// register item icons
		for (auto& itm : m_itemIcons)
			itm.second->Register(m_hud);

		m_hud.m_sellField->SetDropEvent([&](DropField& _this, DraggableTexture& _texture) 
		{
			Game::Item* itm = const_cast<Game::Item*>(static_cast<const Game::Item*>(_texture.GetContent()));
			SellItem(*itm, _this, _texture);
			// money total changed
			UpdateEquipment();
			UpdateUpgradeLabels();
		});

		// sell all option
		m_hud.m_sellAllButton->SetOnMouseUp([this]() 
		{
			// iterate in reverse order so that elements are removed from the end
			auto& texElements = m_hud.m_inventoryField->GetElements();
			for(int j = static_cast<int>(texElements.size()) - 1; j >= 0; --j)
			{
				DraggableTexture* el = texElements[j];
				Game::Item* itm = const_cast<Game::Item*>(static_cast<const Game::Item*>(el->GetContent()));
				SellItem(*itm, *m_hud.m_inventoryField, *el);
				UpdateEquipment();
				UpdateUpgradeLabels();
			}
		});

		UpdateMoneyDipslay();
		// after the ship info text has been set and scaled scale the background
		m_hud.m_shipInfoBackground.SetSize(m_hud.m_shipInfoLabel->GetRectangle() * Vec2(1.144f, 1.0f));
		m_hud.m_moneyBackground.SetSize(Vec2(m_hud.m_moneyLabel->GetRectangle().x, m_hud.m_moneyLabel->GetRectangle().y * 1.2f));
		m_hud.m_moneyLabel->SetPosition(Vec2(0.0f, -0.0f));
		const Vec2 OFFSET = PixelOffset(20, 0);
		const float lineSize = m_hud.m_shipInfoLabel->GetCharSize().y * m_hud.m_shipInfoLabel->GetDefaultSize();
		for (int j = 0; j < Upgrades::COUNT; j++)
		{
			m_hud.m_upgradeLabels[j]->SetPosition(OFFSET + m_hud.m_shipInfoLabel->GetPosition()
				+ Vec2(m_hud.m_shipInfoLabel->GetRectangle().x,
					-m_rows[j] * lineSize));
			m_hud.m_upgradeBtns[j]->SetPosition(OFFSET + m_hud.m_upgradeLabels[j]->GetPosition()
				+ Vec2(m_hud.m_upgradeLabels[j]->GetRectangle().x, -lineSize * 0.57f));
		}
	//	m_hud.m_shipInfoLabel->SetVisible(false);

		// now further equips are done by the player
		m_shouldPlaySounds = true;
	}

	InventoryState::~InventoryState()
	{
		UpdateEquipment();

		m_ship.GetInventory().SetCredits(m_money);

		Game::AudioSystem::Enable3DSounds();
		// restore camera state
		Control::g_camera = m_oldCamera;
	}

	void InventoryState::OnActivate()
	{
		m_grid.SetRotation(m_ship.GetRotation());

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

		Weapon::AccumulatedStats accStats{};
		for (auto& socket : m_ship.GetWeaponSockets())
		{
			if (Weapon* w = static_cast<Weapon*>(socket.GetAttached()))
			{
				auto& stats = w->GetAccumulatedStats();
				accStats.burstDPS += stats.burstDPS;
				accStats.burstEPS += stats.burstEPS;
				accStats.sustainedDPS += stats.sustainedDPS;
				accStats.sustainedEPS += stats.sustainedEPS;
			}
		}

		// show basic stats
		m_hud.m_shipInfoLabel->SetText("ship properties\n"s + '\n'
			+ "energy:   " + ToConstDigit(m_ship.GetEnergy(), 1, 5) + " / " + ToConstDigit(m_ship.GetMaxEnergy(), 1, 5) + '\n'
			+ "recharge: " + ToConstDigit(m_ship.GetEnergyRecharge(), 1, 13) + "\n\n"
			+ "shield:   " + ToConstDigit(m_ship.GetShield(), 1, 5) + " / " + ToConstDigit(m_ship.GetMaxShield(), 1, 5) + '\n'
			+ "recharge: " + ToConstDigit(m_ship.GetShieldRecharge(), 1, 13) + "\n"
			+ "delay:    " + ToConstDigit(m_ship.GetShieldDelay(), 1, 13) + "\n\n"
			+ "hull:     " + ToConstDigit(m_ship.GetHealth(), 1, 5) + " / " + ToConstDigit(m_ship.GetMaxHealth(), 1, 5) + "\n"
			+ "-----------\n"
			+ "burst dps:" + ToConstDigit(accStats.burstDPS, 1, 13) + "\n"
			+ "burst eps:" + ToConstDigit(accStats.burstEPS, 1, 13) + "\n"
			+ "dps:      " + ToConstDigit(accStats.sustainedDPS, 1, 13) + "\n"
			+ "eps:      " + ToConstDigit(accStats.sustainedEPS, 1, 13) + "\n");

		const float values[Upgrades::COUNT]{
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
				m_hud.m_upgradeLabels[i]->SetVisible(true);
				m_hud.m_upgradeBtns[i]->SetActive(true);
				m_hud.m_upgradeBtns[i]->SetVisible(true);
			}
			else
			{
				m_hud.m_upgradeLabels[i]->SetVisible(false);
				m_hud.m_upgradeBtns[i]->SetVisible(false);
				m_hud.m_upgradeBtns[i]->SetActive(false);
			}
		}

	//	m_hud.m_moneyLabel->SetPosition(m_hud.m_shipInfoLabel->GetPosition() + Vec2(0.0f, -m_hud.m_shipInfoLabel->GetRectangle().y));
	}

	// ******************************************************** //
	DropField& InventoryState::CreateItemSocket(ei::Vec2 _position, const Game::Item* _item,
		std::vector<Graphic::DropField*>& _dropField)
	{
		using namespace Game;
		using namespace ei;
		using namespace Utils;

		static const auto& dropSound = Content::GetSound("select_item");

		auto& socketField = m_hud.CreateScreenElement<DropField>("box_uncut", _position, PixelOffset(84, 84), DefP::MidMid, ScreenPosition::Anchor(),
			[&](DropField& _this, DraggableTexture& _tex)
		{
			// do not play the sound when opening the inventory and displaying the current equiped items
			if (m_shouldPlaySounds) AudioSystem::GetGlobalAudio().Play(dropSound);
			// if the field already contains an element put that one back to the main inventory
			if (_this.GetElements().size() > 1)
			{
				DraggableTexture& tex = *_this.GetElements().front();
				m_hud.m_inventoryField->DropElement(tex);
				tex.SetPosition(tex.GetBackupPosition());
			}
			const Game::Item* itm = static_cast<const Game::Item*>(_tex.GetContent());
			itm->Equip(m_ship);

			UpdateEquipment();
			// basic stats may have changed
			UpdateUpgradeLabels();
		});
		_dropField.push_back(&socketField);

		// put in currently equipped item
		if (_item)
		{
			auto it = m_itemIcons.find(static_cast<const Item*>(_item));
			Assert(it != m_itemIcons.end(), "The equipped weapon is not found in the inventory.");
			socketField.DropElement(*it->second);
		}

		return socketField;
	}

	void InventoryState::UpdateEquipment()
	{
		// update changed weapons
		// the first field is the inventory, second the sell field
		for (size_t i = OTHER_FIELDS; i < m_hud.m_weaponFields.size(); ++i)
		{
			const int index = static_cast<int>(i - OTHER_FIELDS);
			auto& elements = m_hud.m_weaponFields[i]->GetElements();
			if (elements.size())
			{
				Game::Weapon* itm = const_cast<Game::Weapon*>(
					static_cast<const Game::Weapon*>(elements.front()->GetContent()));
				// the inventory does not change the weapon's state; but here full access is required
				m_ship.SetWeapon(index, itm);
				// is off by default, only player weapons can be seen effectively
				itm->ShowMuzzleParticles(true);
			}
			else m_ship.SetWeapon(index, nullptr);
		}

		// skip if hud is still in creation
		if (m_hud.m_shieldFields.size() == 3)
		{
			// update shield
			auto& elements = m_hud.m_shieldFields[2]->GetElements();
			if (elements.size())
			{
				const Game::Shield* itm = static_cast<const Game::Shield*>(elements.front()->GetContent());
				m_ship.SetEquipedShield(const_cast<Game::Shield*>(itm));
			}
			else m_ship.SetEquipedShield(nullptr);
		}
	}

	void InventoryState::SellItem(Game::Item& _item, DropField& _source, DraggableTexture& _texture)
	{
		using namespace Game;
		// due to multi-inheritance that Actor part is not in the same location
		Game::Actor* itmActor = _item.GetType() == Item::Type::Weapon ? static_cast<Actor*>(static_cast<Weapon*>(&_item))
			: static_cast<Actor*>(static_cast<Shield*>(&_item));
		m_money += _item.GetValue();
		m_ship.GetInventory().Remove(_item);
		itmActor->Destroy();
		_source.DetachElement(_texture);
		// since the inventory hud is rebuild every time they do not need to be actually destroyed here
		_texture.SetActive(false);
		_texture.SetVisible(false);

	}

	// ******************************************************** //
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

	void InventoryState::UpdateMoneyDipslay()
	{
		using namespace Utils;
		std::string text = "money: " + ToConstDigit(m_money, 0, 5) + "$";
		for (int i = 0; i < Upgrades::COUNT; i++)
		{
			if (m_hud.m_upgradeBtns[i]->GetButtonState() == Graphic::Button::State::MouseOver && m_hud.m_upgradeBtns[i]->IsVisible())
				text += " (-" + ToConstDigit(GetUpgradeCost((Upgrades)i), 0, 6) + "$)";
		}
		m_hud.m_moneyLabel->SetText(text);
	}

	void InventoryState::Process(float _deltaTime)
	{
		// the preview is based on a mouse over that can change at any moment
		UpdateMoneyDipslay();
	}

	void InventoryState::Draw(float _deltaTime)
	{
		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		m_ship.GetGeometryComponent().Draw();

		m_grid.Draw();

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