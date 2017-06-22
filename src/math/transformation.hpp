#pragma once

#include "ei/vector.hpp"

// A simple transformation that combines translation and rotation.
class Transformation
{
public:
	Transformation(const ei::Vec3& _position, const ei::Quaternion& _rotation = ei::qidentity());

	// Get a transformation matrix for this object in the reference system.
	// this transforms local space -> output of _reference
	ei::Mat4x4 Get(const ei::Mat4x4& _reference) const;

	// Returns the combined matrix of this transformation.
	const ei::Mat4x4& Get() { return m_transformation; }

//	void Translate(const ei::Vec3& _direction);
//	void Rotate(const ei::Quaternion& _rotation);

private:
	ei::Mat4x4 m_transformation;
};