#include "specialmove.hpp"
#include "gameplay/core/actor.hpp"
#include "gameplay/elements/blackhole.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "gameplay/scenegraph.hpp"
#include "gameplay/elements/ship.hpp"

namespace Game {

	SpecialMove::SpecialMove(float _cooldown, float _range)
		: m_cooldown(_cooldown),
		m_cooldownMax(_cooldown),
		m_state(State::Ready),
		m_indicator(nullptr),
		m_range(_range)
	{

	}

	void SpecialMove::Process(float _deltaTime)
	{
		if (m_state == State::Charging)
		{
			m_cooldown -= _deltaTime;
			if (m_cooldown <= 0.f) m_state = State::Ready;
		}
	}

	void SpecialMove::Target()
	{
		m_state = State::Targeting;
		OnTarget();
	}

	void SpecialMove::Activate()
	{
		m_state = State::Charging;
		m_cooldown = m_cooldownMax;
		OnActivate();
	}

	void SpecialMove::Dismiss()
	{
		m_state = State::Ready;
		OnDismiss();
	}

	void SpecialMove::SetIndicator(const ei::Vec3& _position, const ei::Quaternion& _rotation)
	{
		m_indicator->SetPosition(_position);
		m_indicator->SetRotation(_rotation);
	}

	bool SpecialMove::IsInRange(const ei::Vec3& _origin) const
	{
		return ei::lensq(_origin - m_indicator->GetPosition()) < m_range * m_range;
	}

	// **************************************************************** //
	void BlackHoleGenerator::TestPlacement(const SceneGraph& _sceneGraph, const Ship& _instigator)
	{
		BlackHole& blackHole = *static_cast<Game::BlackHole*>(m_indicator);
		if (!IsInRange(_instigator.GetPosition()) || blackHole.IsColliding())
		{
			static_cast<Game::BlackHole*>(m_indicator)->SetInvalid(true);
			m_state = State::Invalid;		
		}
		else
		{
			static_cast<Game::BlackHole*>(m_indicator)->SetInvalid(false);
			m_state = State::Active;

		}
	}

	void BlackHoleGenerator::OnTarget()
	{
		m_indicator = new Game::BlackHole(ei::Vec3(0.f), 30.f, 5.f, 10.f);
		Game::FactoryActor::GetThreadLocalInstance().Add(*m_indicator);
	}

	void BlackHoleGenerator::OnActivate()
	{
		static_cast<Game::BlackHole*>(m_indicator)->Activate();
	}

	void BlackHoleGenerator::OnDismiss()
	{
		m_indicator->Destroy();
	}
}