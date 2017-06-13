#include "light.hpp"

namespace Game{

	using namespace Graphic;

	PointLightComponent::PointLightComponent(Actor& _actor, float _radius, Utils::Color8U _color)
		: ActorComponent(_actor),
		m_handle(Graphic::LightSystem::Register(Graphic::LightInfo(_radius, _color)))
	{

	}

	void PointLightComponent::Draw()
	{
		// transform to world space
		m_handle->position = ei::Vec3(m_actor.GetTransformation() * ei::Vec4(m_actor.GetPosition(), 1.f));
	}

	PointLightComponent::~PointLightComponent()
	{
		LightSystem::DeRegister(m_handle);
	}

	PointLight::PointLight(const ei::Vec3& _position, float _radius, Utils::Color8U _color)
		: Actor(_position),
		PointLightComponent(THISACTOR, _radius, _color)
	{

	}
}