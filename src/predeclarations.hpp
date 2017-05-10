/// \file predeclarations.hpp
/// \brief A list of predeclarations for all classes in alphabetical order.

#pragma once

#include "ei/elementarytypes.hpp"

#include "utils/logger.hpp"

// External things
struct GLFWwindow;

// General classes without scopes



namespace Graphic {
	class Effect;
	class Hud;
	class SamplerState;
	class Texture;
	class UniformBuffer;
	class Font;
	class Framebuffer;
	class Texture;
	class PostProcessing;
	class SingleComponentRenderer;
	class DataBuffer;
	class VertexArrayBuffer;

} // namespace Graphic

namespace Input {
	class Camera;
	class Manager;

	enum struct VirtualKey;
} // namespace Input

namespace ei {
	template<typename T, uint M, uint N>
	class Matrix;
	typedef Matrix<int32, 2, 1> IVec2;
	typedef Matrix<int32, 3, 1> IVec3;
	typedef Matrix<int32, 4, 1> IVec4;
	typedef Matrix<float, 2, 1> Vec2;
	typedef Matrix<float, 3, 1> Vec3;
	typedef Matrix<float, 4, 1> Vec4;
	typedef Matrix<float, 4, 4> Mat4x4;
	typedef Matrix<float, 3, 3> Mat3x3;

	template<typename T>
	class TQuaternion;
	typedef TQuaternion<float> Quaternion;
	struct Sphere;
	struct Ray;
} // namespace


namespace Utils {
	class Color32F;
	class Color8U;
} // namespace Utils
