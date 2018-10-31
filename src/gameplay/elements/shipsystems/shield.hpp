#pragma once

#include "gameplay/core/actor.hpp"
#include "item.hpp"

namespace Game {

	class Shield : public TypeItem<Item::Type::Shield>, public Actor
	{
	public:
		typedef std::function<float(Shield&, float)> RechargeFunction;
		typedef std::function<float(Shield&, float)> TakeDamageFunction;
		
		Shield(Item::Quality _quality = Item::Quality::Basic,
			Item::Icon _icon = Item::Icon::DefaultShield,
			const std::string& _name = "Default Shield",
			const std::string& _description = "absorbs some damage",
			float _maxShield = 10.f, float _recharge = 1.f, float _delay = 4.f,
			TakeDamageFunction&& _takeDamageFn = TakeDamageFunction(),
			RechargeFunction&& _rechargeFn = RechargeFunction()
		);

		void Process(float _deltaTime) override;

		// returns the damage that should actually be dealt
		float TakeDamage(float _damage);

		float Recharge(float _deltaTime);

		void Equip(Ship& _ship) const override;
		void UnEquip(Ship& _ship) const override;

		const Ship& GetShip() { return *m_ship; }
	private:
	//	float m_maxShield;
		float m_shieldRecharge;
		float m_shieldDelay;

		TakeDamageFunction m_takeDamageImpl;
		RechargeFunction m_rechargeImpl;

		friend class ShieldTrait;
	};

	class ShieldTrait 
	{
	public:
		static float RechargeDefault(Shield&, float);
		static float TakeDamageDefault(Shield&,float);

		static Shield::TakeDamageFunction ReduceDamageRelative(float _damageReduction);
		static Shield::TakeDamageFunction ReduceDamageAbsolute(float _damageReduction);
		static Shield::TakeDamageFunction ReduceDamageMax(float _damageCap);
		static Shield::TakeDamageFunction TakeDamageEnergy(float _conversionRatio);

		static Shield::TakeDamageFunction AbsorbEnergyRelative(float _energyRatio);

		// health gain in %
		static Shield::RechargeFunction RechargeRepair(float _amount);

		//static Shield::RechargeFunction RechargeNoInterrupt(float _duration);
	};
}