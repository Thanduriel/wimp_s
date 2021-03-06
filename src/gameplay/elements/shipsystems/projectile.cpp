#include "projectile.hpp"
#include "generators/random.hpp"
#include "gameplay/scenegraph.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "../explosion.hpp"
#include "graphic/resources.hpp"

namespace Game {

	using namespace ei;

	const float PARTICLESPAWN = 300.f; // in particles per second

	// ********************************************************************** //
	Projectile::Projectile(const ei::Vec3& _position, const ei::Vec3& _velocity, float _damage, float _lifeTime, DamageType _damageType, bool _explodeOnTimeOut)
		: DynamicActor(_position, Quaternion(Vec3(0.f,0.f,1.f), ei::normalize(_velocity))),
		m_lifeTimeComponent(THISACTOR, _lifeTime, _explodeOnTimeOut),
		m_damage(_damage),
		m_damageType(_damageType)
	{
		m_health = 0.0001f;
		m_canTakeDamage = true;
		SetVelocity(_velocity);
	}

	Projectile::Projectile(const Projectile& _proj)
		: DynamicActor(_proj),
		m_lifeTimeComponent(THISACTOR, _proj.m_lifeTimeComponent),
		m_damage(_proj.m_damage),
		m_damageType(_proj.m_damageType)
	{

	}

	void Projectile::OnCollision(Actor& _other, const HitInfo& _info)
	{
		_other.Damage(m_damage, *this, m_damageType);
	//	Destroy();
	}

	void Projectile::RegisterComponents(SceneGraph& _sceneGraph)
	{
		DynamicActor::RegisterComponents(_sceneGraph);

		_sceneGraph.RegisterComponent(m_lifeTimeComponent);
	}

	// ********************************************************************** //
	const float BOLT_SIZE = 0.8f;
	Bolt::Bolt(const ei::Vec3& _position, const ei::Vec3& _velocity, float _damage, float _lifeTime, DamageType _damageType, const Utils::Color8U& _color)
		: Projectile(_position, _velocity, _damage, _lifeTime, _damageType, false),
		m_particles(THISACTOR, Vec3(0.f), Graphic::Resources::GetTexture("bolt")),
		m_color(_color),
		m_collisionComponent(THISACTOR, BOLT_SIZE*0.2f, CollisionComponent::Type::Any 
			| CollisionComponent::Type::Dynamic | CollisionComponent::Type::Projectile /*, ei::Box(Vec3(sqrt(-BOLT_SIZE/2.f)), Vec3(sqrt(BOLT_SIZE/2.f)))*/)
	{
	}

	Bolt::Bolt(const Bolt& _orig)
		: Projectile(_orig),
		m_particles(THISACTOR, _orig.m_particles),
		m_color(_orig.m_color),
		m_collisionComponent(THISACTOR, _orig.m_collisionComponent)
	{

	}

	void Bolt::Process(float _deltaTime)
	{
		Projectile::Process(_deltaTime);
		const float timeLeft = m_lifeTimeComponent.GetLifeTimeRelative();
		if(timeLeft < 0.95)
			m_collisionComponent.SetBoundingRadius(BOLT_SIZE * (1.7f - timeLeft));
		
		m_particles.AddParticle(Vec3(0.f), //position
			m_color.RGBA(),
			m_damage / 8.f);// size
	}

	void Bolt::RegisterComponents(SceneGraph& _sceneGraph)
	{
		Projectile::RegisterComponents(_sceneGraph);

		_sceneGraph.RegisterComponent(m_particles);
		_sceneGraph.RegisterComponent(m_collisionComponent);
	}

	void Bolt::OnDestroy()
	{
		static const clunk::Sample& sound = Content::GetSound("small_boom");
		FactoryActor::GetThreadLocalInstance().MakeP<Explosion>(m_position, m_damage / 4.f, 0.f, m_color, 
			m_damageType == DamageType::Ion ? "shock" : "shock", sound);
	}

	// ********************************************************************** //
	Rocket::Rocket(const Vec3& _position, const ei::Vec3& _velocity, float _damage, float _lifeTime)
		: Projectile(_position, _velocity, _damage, _lifeTime),
		m_model("missile", THISACTOR),
		m_engineLight(THISACTOR, m_model.GetMesh().GetSocket("thruster")*1.1f, 2.5f, Utils::Color8U(0.4f, 0.2f, 0.9f)),
		m_thrustParticles(THISACTOR, m_model.GetMesh().GetSocket("thruster") * 0.8f),
		m_particleSpawnCount(0.f)
	{
		SetInertiaTensor(m_model.ComputeInertiaTensor(m_mass));
		// a small grace period to not collide with the owner
		GetCollisionComponent().SetType(0);
	}

	Rocket::Rocket(const Rocket& _orig)
		: Projectile(_orig),
		m_engineLight(THISACTOR, _orig.m_engineLight),
		m_thrustParticles(THISACTOR, _orig.m_thrustParticles),
		m_particleSpawnCount(_orig.m_particleSpawnCount),
		m_model(THISACTOR, _orig.m_model)
	{}

	// ********************************************************************** //
	const float TARGETING_STRENGTH = 0.9f;
	const float SETUP_TIME = 0.8f;
	void Rocket::Process(float _deltaTime)
	{
		Projectile::Process(_deltaTime);

		float passed = m_lifeTimeComponent.GetLifeTimeMax() - m_lifeTimeComponent.GetLifeTimeLeft();
		if (passed > SETUP_TIME)
		{
			// activate collision delayed to prevent the rocket from hitting its source
			if(!GetCollisionComponent().GetType()) GetCollisionComponent().SetType(CollisionComponent::Type::Any | CollisionComponent::Type::Solid
								| CollisionComponent::Type::Projectile | CollisionComponent::Type::Dynamic);
			if (m_target && *m_target)
			{
				float l = ei::len(m_velocity);
				// have both acceleration and conversion of TARGETING_STRENGTH in the desired direction
				SetVelocity(GetVelocity() * (1.f - TARGETING_STRENGTH * _deltaTime) + normalize((**m_target).GetPosition() - m_position)
					* _deltaTime * (15.f + l * TARGETING_STRENGTH));
				// movement direction changes -> update rotation
				SetRotation(ei::Quaternion(ei::Vec3(0.f, 0.f, 1.f), GetVelocity()));
			}
		}
		else SetVelocity(GetVelocity() + m_rotationMatrix * Vec3(0.f, 0.f, _deltaTime) * 15.f);

		m_particleSpawnCount += _deltaTime * PARTICLESPAWN;
		static thread_local Generators::RandomGenerator rng(0x614AA);
		while (m_particleSpawnCount > 1.f)
		{
			--m_particleSpawnCount;

			Vec3 dir = (Vec3(0.f, 0.f, -1.f) + rng.Direction() * 0.2f) * rng.Uniform(0.3f, 2.f);
			float col = rng.Uniform(0.2f, 0.7f);
			m_thrustParticles.AddParticleV(rng.Direction() * 0.1f, //position
				dir, //velocity
				2.0f, //life time
				Utils::Color8U(col, col, col, 1.f).RGBA(),
				0.06f); // size
		}
	}

	// ********************************************************************** //
	void Rocket::RegisterComponents(SceneGraph& _sceneGraph)
	{
		Projectile::RegisterComponents(_sceneGraph);

		_sceneGraph.RegisterComponent(m_engineLight);
		_sceneGraph.RegisterComponent(m_thrustParticles);
		_sceneGraph.RegisterComponent(m_model);
	}

	void Rocket::OnDestroy()
	{
		static const clunk::Sample& sound = Content::GetSound("explosion02");
		FactoryActor::GetThreadLocalInstance().MakeP<Explosion>(m_position, 20.f, 0.f, Utils::Color8U(0.7f, 0.8f, 0.4f, 1.f), "mist", sound);
	}
}