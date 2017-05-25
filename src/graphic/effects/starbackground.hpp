#pragma once

#include <vector>
#include "graphic/core/vertexbuffer.hpp"
#include "predeclarations.hpp"
#include "ei/vector.hpp"

namespace Graphic {

	/// \brief A cluster of star systems.
	class Starbackground
	{
	public:
		/// \brief Generates a galaxy at (0,0,0).
		/// \param [in] _stars the amount of stars in the galaxy
		/// \param [in] _size the approximate diameter
		/// \param [in] _ambientStars the amount of stars in infinite distance
		///				that will always be in the same direction
		Starbackground(int _stars, float _size, int _ambientStars = 0);

		/// \brief Draws all background stars of this galaxy.
		void Draw();

	private:
		struct StarVertex
		{
			ei::Vec3 position; ///< position in viewspace
			uint32 color;
			float size;
		};

		/// \brief Converts a given temperature in kelvin into a
		/// to the tempereture coresponding RGB-color.
		/// \details derivation and code found here: http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
		static Utils::Color8U TemperatureToRGB(int _temperature);

		std::vector< StarVertex > m_starSystems;

		Graphic::VertexArrayBuffer m_ambientStars;
	};

}