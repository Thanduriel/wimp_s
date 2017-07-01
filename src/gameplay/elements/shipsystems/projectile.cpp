#include "projectile.hpp"
#include "generators/random.hpp"
#include "gameplay/scenegraph.hpp"
#include "gameplay/elements/factorycomponent.hpp"

namespace Game {

	using namespace ei;

	const Vec3 THRUSTEROFFSET = Vec3(0.f, 0.f, -1.f);
	const float PARTICLESPAWN = 300.f; // in particles per second

	Projectile::Projectile(const ei::Vec3& _position, const ei::Vec3& _velocity, float _lifeTime)
		: DynamicActor(_position, Quaternion(Vec3(0.f,0.f,1.f), _velocity)),
		m_lifeTime(_lifeTime)
	{
		SetVelocity(_velocity);
	}

	void Projectile::Process(float _deltaTime)
	{
		DynamicActor::Process(_deltaTime);

		m_lifeTime -= _deltaTime;
		if (m_lifeTime <= 0.f)
		{
			Destroy();
			FactoryActor::GetThreadLocalInstance().MakeP<Explosion>(m_position);
		}
	}

	// ********************************************************************** //
	Rocket::Rocket(const Vec3& _position, const ei::Vec3 _velocity, float _lifeTime)
		: Projectile(_position, _velocity, _lifeTime),
		m_engineLight(THISACTOR, THRUSTEROFFSET * 1.3f, 2.5f, Utils::Color8U(0.4f, 0.2f, 0.9f)),
		m_thrustParticles(THISACTOR, THRUSTEROFFSET),
		m_mesh(THISACTOR, "testrocket"),
		m_particleSpawnCount(0.f)
	{

	}

	void Rocket::Process(float _deltaTime)
	{
		Projectile::Process(_deltaTime);

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

	void Rocket::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_engineLight);
		_sceneGraph.RegisterComponent(m_thrustParticles);
		_sceneGraph.RegisterComponent(m_mesh);
	}

	// ********************************************************************** //
	Explosion::Explosion(const ei::Vec3& _position)
		: Actor(_position),
		m_light(THISACTOR, Vec3(0.f), 15.f, Utils::Color8U(0.0f, 0.0f, 1.0f)),
		m_particles(THISACTOR, Vec3(0.f)),
		m_lifeTime(5.f)
	{
		for (int i = 0; i < 1000; ++i)
		{
			static Generators::RandomGenerator rng(0x614AA);
			Vec3 dir = rng.Direction() * rng.Uniform(0.5f, 20.f);
			float col = rng.Uniform(0.2f, 0.7f);
			m_particles.AddParticle(Vec3(), //position
				dir, //velocity
				5.0f, //life time
				Utils::Color8U(0.7f, 0.8f, col, 1.f).RGBA(),
				0.2f); // size
		}
	}

	void Explosion::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_light);
		_sceneGraph.RegisterComponent(m_particles);
	}

	void Explosion::Process(float _deltaTime)
	{
		m_lifeTime -= _deltaTime;

		if (m_lifeTime < 0.f) Destroy();
	}
}