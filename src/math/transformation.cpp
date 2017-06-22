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
