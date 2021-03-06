#pragma once

#include <string>
#include "utils/assert.hpp"
#include <cstdint>
#include <jofilelib.hpp>
#include "graphic/core/vertexbuffer.hpp"
#include "graphic/core/effect.hpp"
#include "graphic/core/texture.hpp"
#include "graphic/core/device.hpp"
#include "utils/color.hpp"
#include "screenoverlay.hpp"

namespace Graphic {

	/// \brief A class to load and render a bitmap font.
	class Font
	{
	public:
		/// \brief Load a bitmap font and initialize the render effect.
		Font(const std::string& _fontName);

		void SetShift(unsigned char _char, float _shift);
	private:
		friend class TextRender;

		ei::Vec2 m_sizeTable[256];
		ei::Vec2 m_coordTable[256];
		float m_shiftTable[256]; // the x offset of the next char
		Effect m_effect;
		Texture m_texture;
	};

	// For Font sizes
	struct PixelSize
	{
		constexpr PixelSize(int _size) : m_size(_size) {}

	private:
		friend class TextRender;
		int m_size;
	};
	inline constexpr PixelSize operator""_px(unsigned long long _arg) { return PixelSize(static_cast<int>(_arg)); }

	/// \brief A buffer with a formated string to draw.
	class TextRender : public ScreenPosition
	{
		/// \brief Information required by shader to find and format the
		///		character tile.
		struct CharacterVertex
		{
			ei::Vec2 position;	///< Position on the screen in [-1,1]x[-1,1] where (-1,-1) is the lower left corner
			ei::Vec2 texCoord;	///< Texture position (XY = [0,1]x[0,1]) relative to the lower left corner (0,0)
			ei::Vec2 size;		///< Width and height relative to the texture size [0,1]x[0,1]
			uint32 color;
			float thickness;	///< A number in [0,1] to modify character thickness.
			float scale;		///< Something like the font size.
		};

	public:
		/// \brief Create a text of a specific font.
		TextRender( ei::Vec2 _position = ei::Vec2(0.f), 
			ScreenOverlay::Anchor _anchor = ScreenOverlay::Anchor(), Font* _font = nullptr,
			const std::string& _text = "", DefinitionPoint _defP = DefP::TopLeft);

		void Register(class Hud& _hud) override;
		void Unregister(class Hud& _hud) override;

		/// \brief Draw all buffered texts.
		void Draw() const;

		/// \brief Sets the text and updates the VertexBuffer.
		/// \details chars are one byte, ANSI code 
		/// control chars and formating : 
		/// 13 : line break
		/// "<s sss>" size "sized text" "</s>"
		/// "<c rrr ggg bbb aaa>" "colored text" "</c>"
		/// "<i>" "italic text" "</i>" not yet implemented
		/// "<t  ttt>" "thickness text" "</b>"
		void SetText(const std::string& _text);
		const std::string& GetText() const {return m_text;};

		static std::string GetColoredText(const std::string& _text, const Utils::Color8U& _color);

		bool IsVisible() const { return m_visible; }
		void SetVisible(bool _visible) { m_visible = _visible; }

		/// \brief Sets the Position on the screen.
		void SetPosition(ei::Vec2 _screenPos) override;

		/// \brief Sets the default size of the Textrender which gets used while no control char overrides it
		void SetDefaultSize(float _size);
		void SetDefaultSize(PixelSize _size);
		float GetDefaultSize() const {return m_sizeD;};

		void SetDefaultColor(Utils::Color8U _color) { m_colorD = _color; RenewBuffer(); };

		/// \brief returns the real dimensions the chars are drawn in
		/// \details chars are stretched to preserve the right view on the screen
		ei::Vec2 GetCharSize() const;

		/// \brief returns the size of the smallest rectangle in screen coords in which the displayed text would fit in
		ei::Vec2 GetRectangle() const;

		/// \brief Scales the font size so that the text fills the area on the screen
		void SetRectangle(const ei::Vec2& _rectangle, bool _onlyScaleDown = false);

		/// \brief returns the largest size the string contains(set per <s>)
		float GetMaxSize() const {return m_sizeMax;};

	private:
		void RenewBuffer();
		DefinitionPoint m_defPoint;

		typedef std::string::iterator strIterator;
		/// \brief Resolves control chars and applies them.
		/// \param [in] _start index of '<' in m_text
		bool CntrlChr(strIterator& _i);

		// Advances the iterator looking for a number ['0','9'] preceded by spaces.
		// If no number was found zero is returned.
		int ParseInt(strIterator& _ind);
		const Font& m_font;	///< Reference to the used font.
		std::string m_text;
		bool m_visible;

		VertexArrayBuffer m_characters;
		Utils::Color8U m_color, m_colorD;
		float m_size, m_sizeD;
		float m_thickness, m_thicknessD;

		float m_sizeMax;

		int m_lineCount;		///< Amount of line breaks m_text contains
		ei::Vec2 m_rectangle;		///< size of the area the string possesses on the screen
		ei::Vec2 m_charSize;	///< real x/y size a char is drawn in; x is not reliable since most fonts have differing widths
	};
};