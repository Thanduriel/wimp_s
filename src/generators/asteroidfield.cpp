#include "asteroidfield.hpp"

#include "gameplay/scenegraph.hpp"
#include "gameplay/core/model.hpp"
#include "gameplay/elements/factorycomponent.hpp"
#include "graphic/resources.hpp"
#include "gameplay/content.hpp"

namespace Generators {

	using namespace ei;
	using namespace Game;

	SpaceConstraint::SpaceConstraint(const Vec3& _position, float _radSq)
		: pair(_position, _radSq)
	{
	}

	SpaceConstraint::SpaceConstraint(const CollisionComponent& _source)
		: pair(_source.GetActor().GetPosition(), _source.GetBoundingRadiusSq())
	{
	}

	SpaceConstraint::SpaceConstraint(const Model& _source)
		: SpaceConstraint(_source.GetCollisionComponent())
	{
	}

	enum AsteroidType
	{
		Asteroid01,
		Asteroid02,
		COUNT
	};

	struct AsteroidInfo
	{
		std::string meshName;
		std::string boundingMeshName;
		float health;
		float mass;
	};

	const std::array< AsteroidInfo, AsteroidType::COUNT> ASTEROIDS =
	{ {
		// mesh, bounding mesh, health, mass
		{"asteroid", "asteroidbm", 250.f, 1100.f},
		{"asteroid02", "asteroid02bm", 320.f, 2100.f}
	} };

	class Asteroid : public Model
	{
	public:
		Asteroid(const Vec3& _position, const Quaternion& _rotation, AsteroidType _type)
			: Model(ASTEROIDS[_type].meshName, ASTEROIDS[_type].boundingMeshName, _position, _rotation, ASTEROIDS[_type].mass)
		{
			m_canTakeDamage = true;
			m_maxHealth = ASTEROIDS[_type].health;
			m_health = m_maxHealth;
			GetCollisionComponent().SetMaterial(CollisionComponent::Material::Rock);
		}
	};

	AsteroidField::AsteroidField(const Vec3& _position, float _radius, unsigned _seed)
		: m_position(_position),
		m_radius(_radius),
		m_randomGen(_seed)
	{}

	void AsteroidField::Generate(int _numAsteroids, std::vector<SpaceConstraint>&& _constraints)
	{
		m_restrictedVolumes = std::move(_constraints);

		FactoryActor& factory = FactoryActor::GetThreadLocalInstance();

		// collect mesh data
		std::vector<float> sizes;
		for (auto& mesh : ASTEROIDS)
		{
			float r = Game::Content::GetBoundingMesh(mesh.boundingMeshName).boundingRadius;
			sizes.push_back(r * r);
			
		}

		for (int i = 0; i < _numAsteroids; ++i)
		{
			AsteroidType type = (AsteroidType)m_randomGen.Uniform(0, (int)ASTEROIDS.size()-1);
			Vec3 position = FindPosition(sizes[type]);
			Model* model = new Asteroid(position, m_randomGen.Rotation(), type);
			factory.Add(*model);
			m_restrictedVolumes.emplace_back(position, sizes[type]);
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

	void AsteroidField::AddConstraint(const SpaceConstraint& _constraint)
	{
		m_restrictedVolumes.push_back(_constraint);
	}
}