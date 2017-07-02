#include "explosion.hpp"
#include "../scenegraph.hpp"
#include "generators/random.hpp"

namespace Game {

	using namespace ei;

	// time it takes for particles on average to reach _radius
	const float EXPANSIONTIME = 1.f;

	// ********************************************************************** //
	Explosion::Explosion(const ei::Vec3& _position, float _radius, float _damage)
		: Actor(_position),
		m_lifeTimeComponent(THISACTOR, 2.f),
		m_light(THISACTOR, Vec3(0.f), _radius, Utils::Color8U(0.0f, 0.0f, 1.0f)),
		m_particles(THISACTOR, Vec3(0.f))
	{
		m_canTick = false;

		for (int i = 0; i < 1000; ++i)
		{
			static Generators::RandomGenerator rng(0x614AA);
			float t = EXPANSIONTIME * _radius;
			Vec3 dir = rng.Direction() * rng.Uniform(t - 5.f, t + 5.f);
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
		_sceneGraph.RegisterComponent(m_lifeTimeComponent);
	}

}