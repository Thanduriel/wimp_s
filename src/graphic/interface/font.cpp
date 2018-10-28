#include "font.hpp"
#include "../resources.hpp"
#include "hud.hpp"
#include "utils/color.hpp"
#include <iostream>
using namespace ei;


//#define </cm(r,g,b,a) "</c + (char)r + (char)g + (char)b + (char)a"

namespace Graphic
{
	Font::Font(const std::string& _fontName) :
		m_texture("texture/"+_fontName+".png"),
		m_effect( "shader/font.vs", "shader/font.ps", "shader/font.gs")
	{
		m_effect.SetRasterizerState(RasterizerState(RasterizerState::CULL_MODE::NONE, RasterizerState::FILL_MODE::SOLID));
		m_effect.SetBlendState(BlendState(BlendState::BLEND_OPERATION::ADD, BlendState::BLEND::SRC_ALPHA, BlendState::BLEND::INV_SRC_ALPHA));
		m_effect.SetDepthStencilState(DepthStencilState(DepthStencilState::COMPARISON_FUNC::ALWAYS, false));
		m_effect.BindTexture( "u_characterTex", 7, Graphic::Resources::GetSamplerState(SamplerStates::LINEAR) );
		m_effect.BindUniformBuffer( Graphic::Resources::GetUBO(UniformBuffers::GLOBAL) );

		Jo::Files::HDDFile file("texture/" + _fontName + ".sraw");
		Jo::Files::MetaFileWrapper reader( file, Jo::Files::Format::SRAW );

		auto& PosX = reader[std::string("positionX")];
		auto& PosY = reader[std::string("positionY")];
		auto& sizeX = reader[std::string("sizeX")];
		auto& sizeY = reader[std::string("sizeY")];
		//get values
		for(int i = 0; i < 256; i++)
		{
			m_sizeTable[i] = Vec2((float)sizeX[i],(float)sizeY[i]);
			// Half pixel offset necessary - otherwise rounding errors in shader
			m_coordTable[i] = Vec2(float(PosX[i]) + 0.5f/m_texture.Width(),(float)PosY[i]);
			m_shiftTable[i] = m_sizeTable[i].x;
		}

		SetShift('Î', 0.f);
	}

	void Font::SetShift(unsigned char _char, float _shift)
	{
		m_shiftTable[_char] = _shift;
	}

	// ***************************************************** //
	TextRender::TextRender(ei::Vec2 _position, ScreenOverlay::Anchor _anchor,
		Font* _font, const std::string& _text, DefinitionPoint _defP) :
		ScreenPosition(_position, _anchor),
		m_font(_font ? *_font : Resources::GetFont(Fonts::DEFAULT)),
		m_text(_text),
		m_characters( VertexArrayBuffer::PrimitiveType::POINT, {
				{Graphic::VertexAttribute::VEC2, 11}, {Graphic::VertexAttribute::VEC2, 12},
				{Graphic::VertexAttribute::VEC2, 13}, {Graphic::VertexAttribute::COLOR, 3},
				{Graphic::VertexAttribute::FLOAT, 14}, {Graphic::VertexAttribute::FLOAT, 15}
		} ),
		m_sizeD(1.f),
		m_colorD((uint8)255,(uint8)255,(uint8)255,(uint8)255),
		m_thicknessD(0.7f),
		m_visible(true),
		m_sizeMax(0),
		m_defPoint(_defP)
	{
		m_size = m_sizeD;
		m_color = m_colorD;
		m_thickness = m_thicknessD;

		//calculates the screen ratio
		m_charSize = Vec2(m_font.m_sizeTable[64][0],
			m_font.m_sizeTable[64][1] * (m_font.m_texture.Height()
				/ (float)m_font.m_texture.Width()) * Device::GetAspectRatio());
	}


	void TextRender::SetText(const std::string& _text)
	{ 
		m_text = _text;
		RenewBuffer();
	}

	std::string TextRender::GetColoredText(const std::string& _text, const Utils::Color8U& _color)
	{
		return "<c " + std::to_string(_color.R()) + ' ' + std::to_string(_color.G()) + ' ' + std::to_string(_color.B())
			+ ">" + _text + "</c>";
	}


	void TextRender::SetPosition(Vec2 _screenPos)
	{ 
		ScreenPosition::SetPosition(_screenPos);
		RenewBuffer();
	}

	void TextRender::SetDefaultSize(float _size)
	{
		m_sizeD = _size;
		//only happens on control char endings, thus refresh now
		m_size = m_sizeD;
		RenewBuffer();
	}

	void TextRender::SetDefaultSize(PixelSize _size)
	{
		const float yDim = m_font.m_sizeTable[64][1];
		float pSize = 0.5f * Device::GetBackbufferSize().y * yDim * m_font.m_texture.Height() / (float)m_font.m_texture.Width() * Device::GetAspectRatio();

		SetDefaultSize(_size.m_size / pSize );
	}

	Vec2 TextRender::GetCharSize() const
	{
		return m_charSize;
	}


	Vec2 TextRender::GetRectangle() const
	{
		return m_rectangle;
	}

	void TextRender::SetRectangle(const Vec2& _rectangle, bool _onlyScaleDown)
	{
		const int len = (int)m_text.length();
		int lineCount = 1;
		int charCount = 0;
		int charCountMax = 0;
		for (int i = 0; i < len; i++)
		{
			if (m_text[i] == '<') // skip control chars
			{
				i = m_text.find('>', i + 1);
				continue;
			}
			charCount++;
			if (m_text[i] == '\n')
			{
				if (charCountMax < charCount)
					charCountMax = charCount;
				charCount = 0;
				lineCount++;
			}
		}

		//if the text contains no linebreaks
		if (!charCountMax) charCountMax = charCount;

		const Vec2 captionDim = GetCharSize();

		// in case that the text is to large in any direction scale it down
		if (!_onlyScaleDown || (captionDim[0] * charCountMax * GetDefaultSize() >= _rectangle[0] || captionDim[1] * lineCount >= _rectangle[1]))
		{
			SetDefaultSize(min((float)(_rectangle[0] / (captionDim[0] * charCountMax)),
				(float)(_rectangle[1] / (captionDim[1] * lineCount))));
		}

	}

	// ***************************************************** //
	void TextRender::Draw() const
	{
		if(m_characters.GetNumVertices())
		{
			Graphic::Device::SetEffect( m_font.m_effect );
			Graphic::Device::SetTexture( m_font.m_texture, 7 );
			Graphic::Device::DrawVertices( m_characters, 0, m_characters.GetNumVertices() );
		}
	}

	// ***************************************************** //
	void TextRender::Register(Hud& _hud)
	{
		_hud.RegisterElement(*this);
	}

	void TextRender::Unregister(Hud& _hud)
	{
		_hud.RegisterElement(*this);
	}

	// ***************************************************** //
	bool TextRender::CntrlChr(strIterator& _it)
	{
		++_it;

		switch (*_it)
		{
		case 's': m_size = ParseInt(_it) * 12.5f / 255.f;
			break;
		case 't': m_thickness =(uint8) ParseInt(_it) * 6.25f / 255.f;
			break;
		case 'c': 
		{
			++_it;
			uint8 r = ParseInt(_it);
			uint8 g = ParseInt(_it);
			uint8 b = ParseInt(_it);
			uint8 a = ParseInt(_it);
			m_color = Utils::Color8U(r, g, b, a ? a : 255);
			break;
		}
		case '/': 
			++_it;
			switch (*_it) //statement closed, return to default values
			{
			case 's' : m_size = m_sizeD; 
				break;
			case 'c' : m_color = m_colorD; 
				break;
			case 't' : m_thickness = m_thicknessD; 
				break;
			};
			break;
		default: --_it;  return true;
		}
		//count chars till the formating end: '>'
		while(*_it != '>') ++_it;
		return false;
	}

	// ***************************************************** //
	int TextRender::ParseInt(strIterator& _it)
	{
		while (*_it == ' ') ++_it;

		int res = 0;
		while (*_it >= '0' && *_it <= '9')
		{
			res *= 10;
			res += (int)*_it - 0x30;
			++_it;
		}
		return res;
	}

	// ***************************************************** //
	void TextRender::RenewBuffer()
	{
		//reset the previous build
		m_characters.Clear();
		auto vbGuard = m_characters.GetBuffer(0);
		m_sizeMax = m_size;
		m_color = m_colorD;
		m_size = m_sizeD;
		Vec2 currentPos = m_position;
		//y position is one line down so that m_position represents the upper left corner
		currentPos.y -= m_charSize[1] * m_sizeMax; 

		float maxExpanseX = currentPos[0];
		bool ignoreCntr = false;

		for(auto it = m_text.begin(); it != m_text.end(); ++it)
		{
			CharacterVertex CV;
			CV.scale = m_size; 
			CV.size = m_font.m_sizeTable[(unsigned char)*it];
			CV.texCoord = m_font.m_coordTable[(unsigned char)*it];
			CV.position = currentPos;
			CV.thickness = m_thickness;
			CV.color = m_color.RGBA();

			//line break
			if(*it == '\n')
			{
				if (currentPos[0] > maxExpanseX) maxExpanseX = currentPos[0];
				currentPos[0] = m_position[0]; 
				//m_screenRatio already contains the chars y size because its constant for every char
				currentPos[1] -= m_charSize[1] * m_sizeMax;//offset to lower line space
			}
			else if(*it == '<')
			{
				if (ignoreCntr)
				{
					ignoreCntr = false;
					currentPos[0] += m_font.m_shiftTable[(unsigned char)*it] * m_size;
				} else {
					ignoreCntr = CntrlChr(it);
					continue;
				}
			} 
			else currentPos[0]  += m_font.m_shiftTable[(unsigned char)*it] * m_size;

 			vbGuard->Add(CV);

			//save the greatest size that gets used in the text
			//after checking for cntrl chars
			if(m_size > m_sizeMax && *it != ' ') m_sizeMax = m_size;
		}

		//when no line break happened
		if (currentPos[0] > maxExpanseX) maxExpanseX = currentPos[0];
		//calculate size using the start and end point; the points are always the lower left of the char
		m_rectangle = abs((m_position + Vec2(-maxExpanseX, m_charSize[1] * m_sizeMax * GetCharSize()[1] - currentPos[1])));

		if (m_defPoint != DefP::TopLeft)
		{
			int px = (int)m_defPoint % 3;
			int py = (int)m_defPoint / 3;

			const Vec2 offset = Vec2(-px * 0.5f * m_rectangle.x, py * 0.5f * m_rectangle.y);
			CharacterVertex* cv = static_cast<CharacterVertex*>(vbGuard->GetDirectAccess());
			for (int i = 0; i < vbGuard->GetNumElements(); ++i)
			{
				cv->position += offset;
				++cv;
			}
		}
	}
}; 