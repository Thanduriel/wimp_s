#include "asteroidfield.hpp"

#include "gameplay/scenegraph.hpp"
#include "gameplay/core/model.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "graphic/resources.hpp"

namespace Generators {

	using namespace ei;
	using namespace Game;

	const std::array< std::pair<std::string, std::string>, 2> ASTEROIDS = 
	{ {
		{"asteroid", "asteroidbm"},
		{"asteroid02", "asteroid02bm"}
	} };

	AsteroidField::AsteroidField(const Vec3& _position, float _radius, int _numAsteroids, unsigned _seed)
		: m_position(_position),
		m_radius(_radius),
		m_randomGen(_seed)
	{
		FactoryActor& factory = FactoryActor::GetThreadLocalInstance();

		// collect mesh data
		std::vector<float> sizes;
		for (auto& mesh : ASTEROIDS)
		{
			float r = Graphic::Resources::GetMesh(mesh.first).GetMeshBounds().boundingRadius;
			sizes.push_back(r * r);
			
		}

		for (int i = 0; i < _numAsteroids; ++i)
		{
			int c = m_randomGen.Uniform(0, (int)ASTEROIDS.size()-1);
			Vec3 position = FindPosition(sizes[c]);
			Model* model = new Model(ASTEROIDS[c].first, ASTEROIDS[c].second, position, m_randomGen.Rotation(), m_randomGen.Uniform(900.f, 3500.f));
			factory.Add(*model);
			m_restrictedVolumes.emplace_back(position, sizes[c]);
		}
	}

	Vec3 AsteroidField::FindPosition(float _minRadiusSq)
	{
		Vec3 position;
		bool found;

		do {
			position = m_position + Vec3(m_randomGen.Uniform(-m_radius, m_radius), m_randomGen.Uniform(-m_radius, m_radius), m_randomGen.Uniform(-m_radius, m_radius));

			found = true;
			for (auto& vol : m_restrictedVolumes)
				if (lensq(position - vol.first) < _minRadiusSq + vol.second)
				{
					found = false;
					break;
				}

		} while (!found);

		return position;
	}
}