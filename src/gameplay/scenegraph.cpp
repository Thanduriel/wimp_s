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

using namespace Graphic;

namespace Game {
	// thread safe instances
	std::vector<FactoryActor*> FactoryActor::m_instances;

	SceneGraph::SceneGraph()
	{
		// register general factories
		FactoryActor* factory = new FactoryActor(ei::Vec3(0.f));
		Add(*factory);
		FactoryActor::m_instances.push_back(factory);
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

		// check collisions
		ResolveCollisions();
	}

	void SceneGraph::Draw()
	{
		Graphic::Device::SetEffect(Graphic::Resources::GetEffect(Graphic::Effects::MESH));
		// first step: render geometry to g-Buffer
		for (auto component : m_geometryComponents)
			component->Draw();

		// particles can be illuminated too
		Graphic::ParticleSystems::Manager::Draw(Control::g_camera);

		// apply lights to the frame-buffer
		for (auto component : m_lightComponents)
			component->Draw();
		Graphic::LightSystem::Draw();

		// post lighting 3d effects
		for (auto component : m_markerComponents)
			component->Draw();

		// render the frame-buffer to the hardware back-buffer
		Texture& tex = *Device::GetCurrentFramebufferBinding()->GetColorAttachments().begin()->pTexture;

		Device::BindFramebuffer(nullptr);
		Device::SetEffect(Graphic::Resources::GetEffect(Effects::SCREEN_OUTPUT));
		Device::SetTexture(tex, 0);
		Device::DrawFullscreen();

		// post processing reads from the frame-buffer and writes to the back-buffer
		for (auto component : m_postProcessComponents)
		component->Draw();
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

	void SceneGraph::RegisterComponent(BaseParticleSystemComponent& _component)
	{
		RegisterComponent(component_cast<ConstActorComponent>(_component));
		RegisterBaseComponent(_component);
	}

	void SceneGraph::RegisterComponent(GeometryComponent& _component)
	{
		m_geometryComponents.push_back(&_component);
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
	template <typename T>
	void RemoveDestroyed(std::vector<T*>& _container)
	{
		auto it = std::remove_if(_container.begin(), _container.end(), [](const T* _component)
		{
			return _component->GetActor().IsDestroyed();
		});
		_container.erase(it, _container.end());
	}

	void SceneGraph::CleanUp()
	{
		// todo: test performance; parallelize or perform less frequent

		// unregister components
		RemoveDestroyed(m_geometryComponents);
		RemoveDestroyed(m_lightComponents);
		RemoveDestroyed(m_postProcessComponents);
		RemoveDestroyed(m_markerComponents);
		RemoveDestroyed(m_particleSystemComponents);
		RemoveDestroyed(m_factoryComponents);
		RemoveDestroyed(m_constActorComponents);
		RemoveDestroyed(m_actorComponents);
		RemoveDestroyed(m_collisionComponents);

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
	CollisionComponent* SceneGraph::RayCast(const ei::Ray& _ray, float _maxDist)
	{
		for (CollisionComponent* component : m_collisionComponents)
		{
			float d;
			if (component->RayCastFast(_ray, d) && d < _maxDist && component->RayCast(_ray, d))
			{
				return component;
			}
		}
		return nullptr;
	}

	// *********************************************************** //
	void SceneGraph::ResolveCollisions()
	{
		using namespace ei;
		// todo order components to perform less checks
		for (size_t i = 0; i < m_collisionComponents.size(); ++i)
			for (size_t j = i + 1; j < m_collisionComponents.size(); ++j)
			{
				CollisionComponent* slfComp = m_collisionComponents[i];
				CollisionComponent* othComp = m_collisionComponents[j];
				if (slfComp->GetType() == CollisionComponent::Type::NonPlayer
					&& othComp->GetType() == CollisionComponent::Type::NonPlayer)
					continue;

				Actor& slf = slfComp->GetActor();
				Actor& oth = othComp->GetActor();
				float distSq = ei::lensq(slf.GetPosition()
					- oth.GetPosition());

				if (slfComp->GetBoundingRadiusSq() + othComp->GetBoundingRadiusSq() > distSq)
				{
					// check bounding boxes
					HitInfo hitInfo;

					if (slfComp->Check(*othComp, hitInfo))
					{
						slf.OnCollision(oth);
						oth.OnCollision(slf);

						// some very simple impulse
						DynamicActor* slfDyn = dynamic_cast<DynamicActor*>(&slf);
						DynamicActor* othDyn = dynamic_cast<DynamicActor*>(&oth);
						if (slfDyn && othDyn)
						{
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

							float impulse = -(1 + epsilon) * dot((velocitySlf - velocityOth), normal);
							impulse /= (1 / massSlf + 1 / massOth) + dot(normal, cross(slfDyn->GetInverseInertiaTensor()* cross(radiusSlf, normal), radiusSlf)
								+ cross(othDyn->GetInverseInertiaTensor()* cross(radiusOth, normal), radiusOth));

							slfDyn->AddVelocity(impulse / massSlf * normal);
							othDyn->AddVelocity(-impulse / massOth * normal);

							slfDyn->AddAngularVelocity(slfDyn->GetInverseInertiaTensor() * cross(radiusSlf, normal) * 1.f * impulse);
							othDyn->AddAngularVelocity(othDyn->GetInverseInertiaTensor() * cross(radiusOth, normal) *-1.f * impulse);

						}
					}
				}
			}
	}
}