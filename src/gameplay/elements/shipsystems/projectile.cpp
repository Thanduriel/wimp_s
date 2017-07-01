#include "projectile.hpp"
#include "generators/random.hpp"
#include "gameplay/scenegraph.hpp"

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
		if (m_lifeTime <= 0.f) Destroy();
	}

	// ********************************************************************** //
	Rocket::Rocket(const Vec3& _position, const ei::Vec3 _velocity, float _lifeTime)
		: Projectile(_position, _velocity, _lifeTime),
		m_engineLight(THISACTOR, THRUSTEROFFSET * 1.3f, 1.f, Utils::Color8U(0.4f, 0.2f, 0.9f)),
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
			m_thrustParticles.AddParticle(rng.Direction() * 0.1f, //position
				dir, //velocity
				2.0f, //life time
				Utils::Color8U(0.65f, 0.65f, 0.65f, 0.3f).RGBA(),
				0.06f); // size
		}
	}

	void Rocket::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_engineLight);
		_sceneGraph.RegisterComponent(m_thrustParticles);
		_sceneGraph.RegisterComponent(m_mesh);
	}
}