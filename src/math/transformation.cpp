#include "transformation.hpp"

using namespace ei;

Transformation::Transformation(const ei::Vec3& _position, const ei::Quaternion& _rotation)
	: m_transformation(translation(_position) * Mat4x4(rotation(_rotation)))
{
}

Mat4x4 Transformation::Get(const ei::Mat4x4& _reference) const
{
	return _reference * m_transformation;
}

Vec3 Transformation::GetPosition() const
{
	return Vec3(m_transformation.m03, m_transformation.m13, m_transformation.m23);
}

void Transformation::SetPosition(const ei::Vec3& _position)
{
	m_transformation.m03 = _position.x;
	m_transformation.m13 = _position.y;
	m_transformation.m23 = _position.z;
}


void Transformation::SetRotation(const ei::Vec3& _direction)
{
	ei::Mat3x3 newRot = ei::rotation(Vec3(0.f, 0.f, 1.f), _direction);
	m_transformation.m00 = newRot.m00;
	m_transformation.m01 = newRot.m01;
	m_transformation.m02 = newRot.m02;

	m_transformation.m10 = newRot.m10;
	m_transformation.m11 = newRot.m11;
	m_transformation.m12 = newRot.m12;

	m_transformation.m20 = newRot.m20; 
	m_transformation.m21 = newRot.m21;
	m_transformation.m22 = newRot.m22;
}