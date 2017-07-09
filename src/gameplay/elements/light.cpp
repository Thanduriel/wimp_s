#include "light.hpp"

namespace Game{

	using namespace Graphic;

	PointLightComponent::PointLightComponent(const Actor& _actor, const ei::Vec3& _position,
		float _radius, Utils::Color8U _color)
		: ConstActorComponent(_actor),
		Transformation(_position),
		m_handle(Graphic::LightSystem::Register(Graphic::LightInfo(_radius, _color)))
	{

	}

	PointLightComponent::PointLightComponent(const Actor& _actor, const PointLightComponent& _orig)
		: ConstActorComponent(_actor),
		Transformation(_orig),
		m_handle(Graphic::LightSystem::Register(*_orig.m_handle))
	{}

	void PointLightComponent::Draw()
	{
		// transform to world space
		// todo optimize for null vector (with w = 1)
		m_handle->position = ei::Vec3(Transformation::Get(m_actor.GetTransformation()) * ei::Vec4(0.f, 0.f, 0.f, 1.f));
	}

	PointLightComponent::~PointLightComponent()
	{
		LightSystem::DeRegister(m_handle);
	}
}