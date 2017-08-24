#pragma once

#include "ei/vector.hpp"
#include "random.hpp"

namespace Game {
	class SceneGraph;
}

namespace Generators {

	class AsteroidField
	{
	public:
		AsteroidField(const ei::Vec3& _position, float _radius, int _numAsteroids, unsigned _seed = 0x832af);

		// Finds a suitable position that is not occupied with atleast space for a sphere with _minRadius.
		ei::Vec3 FindPosition(float _minRadiusSq);
	private:
		RandomGenerator m_randomGen;
		float m_radius;
		ei::Vec3 m_position;
		std::vector<std::pair<ei::Vec3, float>> m_restrictedVolumes;
	};
}