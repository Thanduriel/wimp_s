#include "projectile.hpp"
#include "generators/random.hpp"
#include "gameplay/scenegraph.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "../explosion.hpp"

namespace Game {

	using namespace ei;

	const float PARTICLESPAWN = 300.f; // in particles per second

	// ********************************************************************** //
	Projectile::Projectile(const ei::Vec3& _position, const ei::Vec3& _velocity, const std::string& _mesh, float _damage, float _lifeTime)
		: Model(_mesh, _position, Quaternion(Vec3(0.f,0.f,1.f), _velocity)),
		m_lifeTimeComponent(THISACTOR, _lifeTime),
		m_damage(_damage)
	{
		SetVelocity(_velocity);
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
		Model::RegisterComponents(_sceneGraph);

		_sceneGraph.RegisterComponent(m_lifeTimeComponent);
	}

	// ********************************************************************** //
	const Vec3 THRUSTEROFFSET = Vec3(0.f, 0.f, -1.f);

	Rocket::Rocket(const Vec3& _position, const ei::Vec3& _velocity, float _damage, float _lifeTime)
		: Projectile(_position, _velocity, "testrocket", _damage, _lifeTime),
		m_engineLight(THISACTOR, THRUSTEROFFSET * 1.3f, 2.5f, Utils::Color8U(0.4f, 0.2f, 0.9f)),
		m_thrustParticles(THISACTOR, THRUSTEROFFSET),
		m_particleSpawnCount(0.f)
	{

	}

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
			m_thrustParticles.AddParticle(rng.Direction() * 0.1f, //position
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
	}
}