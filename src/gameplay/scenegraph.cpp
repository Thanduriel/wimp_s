#include "scenegraph.hpp"

#include "graphic/core/device.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/framebuffer.hpp"
#include "ei/3dintersection.hpp"

#include "gameplay/elements/light.hpp"
#include "gameplay/elements/blackhole.hpp"
#include "gameplay/elements/particlesystemcomponent.hpp"
#include "elements/factorycomponent.hpp"
#include "elements/collisioncomponent.hpp"
#include "elements/shieldcomponent.hpp"
#include "elements/shipsystems/projectile.hpp"
#include "elements/singlesound.hpp"

using namespace Graphic;

namespace Game {
	// thread safe instances
	std::vector<FactoryActor*> FactoryActor::m_instances;

	SceneGraph::SceneGraph()
	{
		// register general factories
		FactoryActor* factory = new FactoryActor(ei::Vec3(0.f));
		Add(*factory);
		FactoryActor::m_instances.clear();
		FactoryActor::m_instances.push_back(factory);
	}

	void SceneGraph::Add(Actor& _element)
	{
		// ownership is taken here
		m_actors.emplace_back(&_element);

		// let the actor register its own components
		_element.RegisterComponents(*this);
	}

	void SceneGraph::Remove(Actor& _element)
	{
		auto it = std::find_if(m_actors.begin(), m_actors.end(), [&](const std::unique_ptr<Actor>& _ptr) 
		{
			return _ptr.get() == &_element;
		});
		Assert(it != m_actors.end(), "This actor is not known by the SceneGraph.");

		(*it).release();
		(*it) = std::move(m_actors.back());
		m_actors.pop_back();
	}

	void SceneGraph::Process(float _deltaTime, float _realdTime)
	{
		// for now all actors are processed
		// this could be optimized by having an extra container for this
		for (auto& actor : m_actors)
			actor->Process(_deltaTime);

		for (auto component : m_constActorComponents)
			component->ProcessComponent(_deltaTime);
		for (auto component : m_actorComponents)
			component->ProcessComponent(_deltaTime);

		Graphic::ParticleSystems::Manager::Process(_deltaTime);

		for (auto component : m_markerComponents)
			component->ProcessComponent(_realdTime);

		SortAxis();
		// check collisions
		ResolveCollisions();
		for (auto component : m_sceneComponents)
			if(component->IsActive()) component->ProcessComponent(_deltaTime, *this);
	}

	void SceneGraph::Draw()
	{
		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		// first step: render geometry to g-Buffer
		auto geometry = FrustumQuery(Control::g_camera.GetViewFrustum(), m_geometryComponents.begin(), m_geometryComponents.end());
		for (auto component : geometry)
			component->Draw();

		// particles can be illuminated too
		Graphic::ParticleSystems::Manager::Draw(Control::g_camera);

		// shields
		Device::SetEffect(Resources::GetEffect(Effects::SHIELD));
		Device::SetTexture(Resources::GetTexture("noise"), 1);
		for (auto component : m_shieldComponents)
			if (component->IsActive()) component->Draw();

		// apply lights to the frame-buffer
		for (auto component : m_lightComponents)
			component->Draw();
		Graphic::LightSystem::Draw();

		// post lighting 3d effects
		for (auto component : m_markerComponents)
			component->Draw();

		// switch to back-buffer to perform post processing without altering the source material
		Device::DrawFramebufferToBackbuffer();

		// post processing reads from the frame-buffer and writes to the back-buffer
		for (auto component : m_postProcessComponents)
			if(component->IsActive()) component->Draw();
	}

	// *********************************************************** //
	void SceneGraph::RegisterComponent(ConstActorComponent& _component)
	{
		_component.OnRegister();
		if(_component.m_canTick) m_constActorComponents.push_back(&_component);
	}
	void SceneGraph::RegisterComponent(ActorComponent& _component)
	{
		_component.OnRegister();
		if (_component.m_canTick) m_actorComponents.push_back(&_component);
	}

	void SceneGraph::RegisterComponent(SceneComponent& _sceneComponent)
	{
		m_sceneComponents.push_back(&_sceneComponent);
		RegisterBaseComponent(_sceneComponent);
	}

	void SceneGraph::RegisterComponent(BaseParticleSystemComponent& _component)
	{
	//	RegisterComponent(component_cast<ConstActorComponent>(_component));
		RegisterBaseComponent(_component);
	}

	void SceneGraph::RegisterComponent(GeometryComponent& _component)
	{
		m_geometryComponents.push_back(&_component);
		RegisterBaseComponent(_component);
	}

	void SceneGraph::RegisterComponent(ShieldComponent& _component)
	{
		m_shieldComponents.push_back(&_component);
		RegisterBaseComponent(_component);
	}
	

	void SceneGraph::RegisterComponent(PointLightComponent& _component)
	{
		m_lightComponents.push_back(&_component);
		RegisterBaseComponent(_component);
	}

	void SceneGraph::RegisterComponent(PostProcessComponent& _component)
	{
		m_postProcessComponents.push_back(&_component);
		RegisterBaseComponent(_component);
	}

	void SceneGraph::RegisterComponent(MarkerComponent& _component)
	{
		m_markerComponents.push_back(&_component);
		RegisterBaseComponent(_component);
	}

	void SceneGraph::RegisterComponent(FactoryComponent& _component)
	{
		m_factoryComponents.push_back(&_component);
		RegisterBaseComponent(_component);
	}

	void SceneGraph::RegisterComponent(CollisionComponent& _component)
	{
		m_collisionComponents.push_back(&_component);
		RegisterBaseComponent(_component);
	}

	// *********************************************************** //
	void SceneGraph::SortAxis()
	{
		std::sort(m_collisionComponents.begin(), m_collisionComponents.end(), [](const CollisionComponent* _lhs, const CollisionComponent* _rhs)
		{
			return _lhs->m_AABB.max.x < _rhs->m_AABB.max.x;
		});

		int n = (int)m_collisionComponents.size() - 1;
		m_collisionComponents[n]->m_minOfAllMin = m_collisionComponents[n]->m_AABB.min.x;
		for (--n; n >= 0; --n)
		{
			float globalMin = ei::min(m_collisionComponents[n]->m_AABB.min.x, m_collisionComponents[n + 1]->m_minOfAllMin);
			m_collisionComponents[n]->m_minOfAllMin = globalMin;
		}
	}

	// *********************************************************** //
	template <typename T>
	void RemoveDestroyed(std::vector<T>& _container)
	{
		auto it = std::remove_if(_container.begin(), _container.end(), [](const T& _component)
		{
			return _component->GetActor().IsDestroyed();
		});
		_container.erase(it, _container.end());
	}

	void SceneGraph::CleanUp()
	{
		// todo: test performance; parallelize or perform less frequent

		// clean up particles
		Graphic::ParticleSystems::Manager::CleanUp();
		
		// unregister components
		RemoveDestroyed(m_geometryComponents);
		RemoveDestroyed(m_shieldComponents);
		RemoveDestroyed(m_lightComponents);
		RemoveDestroyed(m_postProcessComponents);
		RemoveDestroyed(m_markerComponents);
		RemoveDestroyed(m_particleSystemComponents);
		RemoveDestroyed(m_factoryComponents);
		RemoveDestroyed(m_constActorComponents);
		RemoveDestroyed(m_actorComponents);
		RemoveDestroyed(m_collisionComponents);
		RemoveDestroyed(m_sceneComponents);
		RemoveDestroyed(m_dynamicComponents);

		// actual destruction is last
		auto it = std::remove_if(m_actors.begin(), m_actors.end(), [](const std::unique_ptr<Actor>& _actor)
		{
			return _actor->IsDestroyed();
		});
		m_actors.erase(it, m_actors.end());
	}

	// *********************************************************** //
	void SceneGraph::AddActors()
	{
		// collect actors created by all known factories
		// it is possible that new factories are added, making any iterator invalid
		size_t size = m_factoryComponents.size();
		for (size_t i = 0; i < size; ++i)
		{
			auto& container = m_factoryComponents[i]->m_createdActors;
			while (container.size())
			{
				Add(*container.back().release());
				container.pop_back();
			}
		}
	}

	// *********************************************************** //
	Actor* SceneGraph::RayCast(const ei::Ray& _ray, float _maxDist, uint32_t _type) const
	{
		using namespace ei;

		// flip ray to always have increasing x coordinates
		Vec3 origin;
		Vec3 direction;
		if (_ray.direction.x < 0.f) { origin = _ray.origin + _ray.direction * _maxDist;  direction = -_ray.direction; } 
		else { origin = _ray.origin; direction = _ray.direction; }

		// split into boxes where the ray is the diagonal
		const int BOX_SIZE = 64;
		int numSplits = max(1, int(_maxDist) / BOX_SIZE);
		Vec3 offset = _maxDist / numSplits * direction;
		Vec3 rayEnd = origin + offset;
		Box box(min(origin, rayEnd), max(origin, rayEnd));

		auto startit = std::lower_bound(m_collisionComponents.begin(), m_collisionComponents.end(), box.min.x, [](const CollisionComponent* _i, const float _ref) 
		{ return _i->m_AABB.max.x < _ref; });
		if (startit == m_collisionComponents.end()) return nullptr;

		// Check all the CollisionComponents inside box and return closest intersection
		Actor* closestHit = nullptr;
		for (int step = 0; step < numSplits; ++step)
		{
			auto it = startit;
			// Iterate as long as the element intersects in x
			while ((*it)->m_minOfAllMin <= box.max[0])
			{
				// The current element intersects in x direction. Does it also intersect
				// in the others?
				if ((*it)->m_AABB.min[1] < box.max[1] && (*it)->m_AABB.max[1] > box.min[1] &&
					(*it)->m_AABB.min[2] < box.max[2] && (*it)->m_AABB.max[2] > box.min[2] &&
					(*it)->GetType() & _type)
				{
					float d;
					// todo: verify that the precheck with RayCastFast is an overall performance increase
					if ((*it)->RayCastFast(_ray, d) && d < _maxDist && (*it)->RayCast(_ray, d))
					{
						closestHit = &(*it)->GetActor();
					}
				}
				++it;
				if (it == m_collisionComponents.end())
					break;
				if ((*startit)->m_AABB.max[0] <= box.max[0])
					++startit;
			}
			// Ray march to a next box segment
			box.min = rayEnd;
			rayEnd = rayEnd + offset;
			box.max = max(box.min, rayEnd);
			box.min = min(box.min, rayEnd);
		}

		return closestHit;
	}
	// *********************************************************** //
	std::vector<std::pair<Actor*, float>> SceneGraph::SphereQuery(const ei::Sphere& _sphere, CollisionFlags _flags) const
	{
		std::vector<std::pair<Actor*, float>> results;

		auto it = std::upper_bound(m_collisionComponents.begin(), m_collisionComponents.end(), _sphere.center.x - _sphere.radius,
			[](const float _ref, const CollisionComponent* _comp)
		{
			return _comp->m_AABB.max.x > _ref;
		});

		float upper = _sphere.center.x + _sphere.radius;
		float rSqr = _sphere.radius * _sphere.radius;
		for (; it != m_collisionComponents.end() && (*it)->m_minOfAllMin < upper; ++it)
		{
			Actor& actor = (*it)->GetActor();
			float dSqr = ei::lensq(actor.GetPosition() - _sphere.center);
			if (dSqr < rSqr && (*it)->GetType() & _flags)
				results.emplace_back(&(*it)->GetActor(), dSqr);
		}

		return results;
	}

	// *********************************************************** //
	void SceneGraph::ResolveCollisions()
	{
		using namespace ei;
		for (int i = (int)m_collisionComponents.size()-1; i >= 0; --i)
		{
			CollisionComponent* slfComp = m_collisionComponents[i];
			// skip if the component can not collide
			if (!slfComp->GetType() & CollisionComponent::Type::Any) continue;

			// only need to check components where the AABB overlaps on the x axis
			for (int j = i - 1; j >= 0 
				&& m_collisionComponents[j]->m_AABB.max.x > slfComp->m_AABB.min.x; --j)
			{
				CollisionComponent* othComp = m_collisionComponents[j];

				Actor& slf = slfComp->GetActor();
				Actor& oth = othComp->GetActor();

				if (!othComp->GetType() & CollisionComponent::Type::Any) continue;
				// two projectiles with the same owner can not collide
				if (slfComp->GetType() & CollisionComponent::Type::Projectile & othComp->GetType()
					&& static_cast<Projectile*>(&slf)->GetOwner() == static_cast<Projectile*>(&oth)->GetOwner())
					continue;

				const float distSq = ei::lensq(slf.GetPosition()
					- oth.GetPosition());

				if (slfComp->GetBoundingRadiusSq() + othComp->GetBoundingRadiusSq() > distSq)
				{
					// check bounding boxes
					HitInfo hitInfo;

					if (slfComp->Check(*othComp, hitInfo))
					{
						slf.OnCollision(oth, hitInfo);
						oth.OnCollision(slf, hitInfo);
						// apply some damage to kill projectiles
						// todo: move this back closer to Projectile
						slf.Damage(0.001f, oth);
						oth.Damage(0.001f, slf);

						static const uint32_t PHYSICSBODY = CollisionComponent::Type::Solid | CollisionComponent::Type::Dynamic;
						// resolve impulse
						if ((slfComp->GetType() & PHYSICSBODY) == PHYSICSBODY && (othComp->GetType() & PHYSICSBODY) == PHYSICSBODY)
						{
							DynamicActor* slfDyn = static_cast<DynamicActor*>(&slf);
							DynamicActor* othDyn = static_cast<DynamicActor*>(&oth);

							Vec3 radiusSlf = hitInfo.position - slf.GetPosition(); //point
							Vec3 radiusOth = hitInfo.position - oth.GetPosition();
							float massSlf = slfDyn->GetMass();
							float massOth = othDyn->GetMass();

							Vec3 velocitySlf = slfDyn->GetVelocity() + cross(slfDyn->GetAngularVelocity(), radiusSlf);
							Vec3 velocityOth = othDyn->GetVelocity() + cross(othDyn->GetAngularVelocity(), radiusOth);

							Vec3 normal = normalize(hitInfo.normal);

							//check that they are really closing and not just intersecting from a previous crash
							if (dot((velocitySlf - velocityOth), normal) >= 0.f) return;

							float epsilon = 0.04f;

							Vec3 rotInertialSlf = slfDyn->GetInverseInertiaTensor()* cross(radiusSlf, normal);
							Vec3 rotInertiaOth = othDyn->GetInverseInertiaTensor() * cross(radiusOth, normal);
							float impulse = -(1 + epsilon) * dot((velocitySlf - velocityOth), normal);
							impulse /= (1 / massSlf + 1 / massOth) + dot(normal, cross(rotInertialSlf, radiusSlf)
								+ cross(rotInertiaOth, radiusOth));

							slfDyn->AddVelocity(impulse / massSlf * normal);
							othDyn->AddVelocity(-impulse / massOth * normal);

							slfDyn->AddAngularVelocity(rotInertialSlf * 1.f * impulse);
							othDyn->AddAngularVelocity(rotInertiaOth *-1.f * impulse);

							float damage = impulse * epsilon * 0.11f;
							slfDyn->Damage(damage, *othDyn);
							othDyn->Damage(damage, *slfDyn);

							if (damage < 2.f) continue;
							const clunk::Sample* sound = slfComp->GetMaterialSound(*othComp);
							if (sound)
							{
								FactoryActor::GetThreadLocalInstance().Make<SingleSoundActor>(hitInfo.position, *sound);
							}
						}
					}
				}
			}
		}
	}

	// *********************************************************** //
	std::vector<GeometryComponent*> SceneGraph::FrustumQuery(const ei::FastFrustum& _frustum,
		GeometryComponentIt _begin, GeometryComponentIt _end)
	{
		using namespace ei;
		std::vector<GeometryComponent*> results;

		for (auto it = _begin; it != _end; ++it)
			if (ei::intersects(Sphere((*it)->GetActor().GetPosition(), (*it)->GetMesh().GetMeshBounds().boundingRadius), _frustum))
				results.push_back(*it);

		return std::move(results);
	}
}