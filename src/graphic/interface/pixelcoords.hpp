#include <ei/vector.hpp>
#include "../core/device.hpp"

namespace Graphic{

	//describes a global position on the screen
	struct PixelCoord : public ei::IVec2
	{
		using ei::IVec2::Matrix;

		operator ei::Vec2() const { return *this * 2.f / Device::GetBackbufferSize() - ei::Vec2(1.f,1.f); }
	};

	//use this for pixel offsets and sizes
	struct PixelOffset : private ei::IVec2
	{
		using ei::IVec2::Matrix;

		operator ei::Vec2() const { return *this * 2.f / Device::GetBackbufferSize(); }
	};
}	