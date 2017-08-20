#include "explosion.hpp"
#include "../scenegraph.hpp"
#include "generators/random.hpp"
#include "graphic/resources.hpp"

namespace Game {

	using namespace ei;

	// time it takes for particles on average to reach _radius
	const float EXPANSIONTIME = 1.f;

	// ********************************************************************** //
	Explosion::Explosion(const ei::Vec3& _position, float _radius, float _damage, Utils::Color8U _color)
		: Actor(_position),
		m_lifeTimeComponent(THISACTOR, 0.9f),
		m_light(THISACTOR, Vec3(0.f), _radius, Utils::Color8U(0.0f, 0.0f, 1.0f)),
		m_particles(THISACTOR, Vec3(0.f), Graphic::Resources::GetTexture("mist")),
		m_radius(_radius)
	{
		for (int i = 0; i < _radius * 18.f; ++i)
		{
			static thread_local Generators::RandomGenerator rng(0x614AA);
			float t = EXPANSIONTIME * _radius;
			Vec3 dir = rng.Direction() * rng.Uniform(t - 2.f, t + 2.f);
		//	float col = rng.Uniform(0.2f, 0.7f);
			m_particles.AddParticle(Vec3(), //position
				dir, //velocity
				1.2f + rng.Uniform(0.f, 0.7f), //life time
				_color.RGBA(),
				0.9f); // size
		}
	}

	void Explosion::Process(float _deltaTime)
	{
		float x = m_lifeTimeComponent.GetLifeTimeLeft() / m_lifeTimeComponent.GetLifeTimeMax();
		m_light.GetLight()->radius = x * m_radius;
	}

	void Explosion::RegisterComponents(SceneGraph& _sceneGraph)
	{
		_sceneGraph.RegisterComponent(m_light);
		_sceneGraph.RegisterComponent(m_particles);
		_sceneGraph.RegisterComponent(m_lifeTimeComponent);
	}

}