#include "light.hpp"

namespace Game{

	using namespace Graphic;

	PointLight::PointLight(const ei::Vec3 _position, float _radius, Utils::Color8U _color)
		: Actor(_position, ei::qidentity()),
		m_handle(Graphic::LightSystem::Register(Graphic::LightInfo(_radius, _color)))
	{

	}

	void PointLight::Process(float _deltaTime)
	{
		// transform to world space
		m_handle->position = ei::Vec3(m_transformation * ei::Vec4(m_position, 1.f));
	}

	PointLight::~PointLight()
	{
		LightSystem::DeRegister(m_handle);
	}
}