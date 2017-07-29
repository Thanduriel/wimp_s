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
	Projectile::Projectile(const ei::Vec3& _position, const ei::Vec3& _velocity, float _damage, float _lifeTime)
		: DynamicActor(_position, Quaternion(Vec3(0.f,0.f,1.f), _velocity)),
		m_lifeTimeComponent(THISACTOR, _lifeTime),
		m_damage(_damage)
	{
		SetVelocity(_velocity);
	}

	Projectile::Projectile(const Projectile& _proj)
		: DynamicActor(_proj),
		m_lifeTimeComponent(THISACTOR, _proj.m_lifeTimeComponent),
		m_damage(_proj.m_damage)
	{

	}

	void Projectile::OnDestroy()
	{
		FactoryActor::GetThreadLocalInstance().MakeP<Explosion>(m_position, 20.f, 0.f);
	}

	void Projectile::OnCollision(Actor& _other)
	{
		_other.Damage(m_damage, *this);
		Destroy();
	}

	void Projectile::RegisterComponents(SceneGraph& _sceneGraph)
	{
		DynamicActor::RegisterComponents(_sceneGraph);

		_sceneGraph.RegisterComponent(m_lifeTimeComponent);
	}

	// ********************************************************************** //
	const float BOLT_SIZE = 0.2f;
	Bolt::Bolt(const ei::Vec3& _position, const ei::Vec3& _velocity, float _damage, float _lifeTime)
		: Projectile(_position, _velocity, _damage, _lifeTime),
		m_particles(THISACTOR, Vec3(0.f), Graphic::Resources::GetTexture("bolt")),
		m_collisionComponent(THISACTOR, BOLT_SIZE /*, ei::Box(Vec3(sqrt(-BOLT_SIZE/2.f)), Vec3(sqrt(BOLT_SIZE/2.f)))*/)
	{
	}

	Bolt::Bolt(const Bolt& _orig)
		: Projectile(_orig),
		m_particles(THISACTOR, _orig.m_particles),
		m_collisionComponent(THISACTOR, _orig.m_collisionComponent)
	{

	}

	void Bolt::Process(float _deltaTime)
	{
		Projectile::Process(_deltaTime);

		m_particles.AddParticle(Vec3(0.f), //position
			Utils::Color8U(0.5f, 1.0f, 0.5f, 1.0f).RGBA(),
			0.7f);// size
	}

	void Bolt::RegisterComponents(SceneGraph& _sceneGraph)
	{
		Projectile::RegisterComponents(_sceneGraph);

		_sceneGraph.RegisterComponent(m_particles);
		_sceneGraph.RegisterComponent(m_collisionComponent);
	}

	// ********************************************************************** //
	Rocket::Rocket(const Vec3& _position, const ei::Vec3& _velocity, float _damage, float _lifeTime)
		: Projectile(_position, _velocity, _damage, _lifeTime),
		m_model(THISACTOR, "testrocket"),
		m_engineLight(THISACTOR, m_model.GetMesh().GetSocket("thruster")*1.1f, 2.5f, Utils::Color8U(0.4f, 0.2f, 0.9f)),
		m_thrustParticles(THISACTOR, m_model.GetMesh().GetSocket("thruster") * 0.8f),
		m_particleSpawnCount(0.f)
	{
		SetInertiaTensor(m_model.ComputeInertiaTensor(m_mass));
	}

	Rocket::Rocket(const Rocket& _orig)
		: Projectile(_orig),
		m_engineLight(THISACTOR, _orig.m_engineLight),
		m_thrustParticles(THISACTOR, _orig.m_thrustParticles),
		m_particleSpawnCount(_orig.m_particleSpawnCount),
		m_model(THISACTOR, _orig.m_model)
	{}

	// ********************************************************************** //
	void Rocket::Process(float _deltaTime)
	{
		Projectile::Process(_deltaTime);

		SetVelocity(GetVelocity() + m_rotationMatrix * Vec3(0.f, 0.f, _deltaTime) * 5.f);

		m_particleSpawnCount += _deltaTime * PARTICLESPAWN;
		static Generators::RandomGenerator rng(0x614AA);
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
}