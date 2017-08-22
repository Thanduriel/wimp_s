#include "hudelements.hpp"
#include "GLFW/glfw3.h"
#include "pixelcoords.hpp"
#include "hud.hpp"
#include "control/input.hpp"
#include <algorithm>

using namespace ei;

namespace Graphic
{
	ScreenTexture::ScreenTexture(const std::string& _name, Vec2 _position, Vec2 _size,
		DefinitionPoint _def, Anchor _anchor,
		std::function<void()> _OnMouseUp) :
		ScreenOverlay(_position, _size, _def, _anchor)
	{
		Jo::Files::MetaFileWrapper& posMap = Resources::GetTextureMap();
		m_vertex.texCoord = Vec2((float)posMap.RootNode[_name][std::string("positionX")], (float)posMap.RootNode[_name][std::string("positionY")]);
		m_vertex.size = Vec2((float)posMap.RootNode[_name][std::string("sizeX")], (float)posMap.RootNode[_name][std::string("sizeY")]);
		m_textureSize = m_vertex.size;
		m_texturePosition = m_vertex.texCoord;
		if (Vec2(0.f) == _size)
		{
			Vec2 screenSize(posMap.RootNode[std::string("width")].Get(132), posMap.RootNode[std::string("height")].Get(132));
			screenSize *= m_vertex.size; //pixel size of this texture
			m_vertex.screenSize = PixelOffset(screenSize);

			ScreenOverlay::SetSize(m_vertex.screenSize);
		}
		else
			m_vertex.screenSize = _size;
		m_vertex.position = m_position;
		m_vertex.color = Utils::Color8U(255_uc, 255_uc, 255_uc);
	}

	void ScreenTexture::Register(Hud& _hud)
	{
		_hud.RegisterElement(*this);
	}

	void ScreenTexture::Unregister(Hud& _hud)
	{
		_hud.UnregisterElement(*this);
	}

	void ScreenTexture::SetPosition(Vec2 _pos)
	{
		ScreenOverlay::SetPosition(_pos);
		m_vertex.position = m_position;// + Math::Vec2(0.f, m_vertex.screenSize[1]/2.f);
	}

	// ************************************************************************ //

	void ScreenTexture::SetSize(Vec2 _size)
	{
		ScreenOverlay::SetSize(_size);
		m_vertex.screenSize = _size;
	}

	// ************************************************************************ //

	void ScreenTexture::Scale(Vec2 _scale)
	{
		ScreenOverlay::Scale(_scale);
		m_vertex.screenSize = m_size;
		m_vertex.position = m_position;
	}

	// ************************************************************************ //
	void ScreenTexture::SetTextureRect(ei::Vec2 _scale, Vec2 _pos)
	{
		m_vertex.size = m_textureSize * _scale;
		m_vertex.texCoord = m_texturePosition + m_textureSize * _pos;
	}

	// ************************************************************************ //

	DropField::DropField(const std::string& _name, Vec2 _position, Vec2 _size,
		DefinitionPoint _def, Anchor _anchor, DropEventFn _onDrop)
		: ScreenTexture(_name, _position, _size, _def, _anchor),
		m_onDrop(_onDrop)
	{
	}

	void DropField::MouseEnter()
	{
		ScreenOverlay::MouseEnter();

		m_state = State::MouseOver;
	}

	void DropField::MouseLeave()
	{
		ScreenOverlay::MouseLeave();

		m_state = State::Base;
	}

	void DropField::DropElement(DraggableTexture& _element)
	{
		if (_element.GetParentField() == this)
			return;
		else if (_element.GetParentField() != nullptr)
			_element.GetParentField()->DetachElement(_element);

		if (m_onDrop) m_onDrop(*this, _element);
		AppendElement(_element);
	}

	void DropField::AppendElement(DraggableTexture& _element)
	{
		//Set backup position to a position inside of the field
		Vec2 newPos = GetScreenSpacePosition() + 0.5f * Vec2(_element.GetSize().x, -_element.GetSize().y);
		//Assumption: All draggable texture have the same width and height
		int maxPerRow = (int)(GetSize().x / _element.GetSize().x);
		int row = (int)(m_elements.size() / maxPerRow);
		int column = m_elements.size() - row * maxPerRow;
		newPos.x += column * _element.GetSize().x;
		newPos.y -= row * _element.GetSize().y;
		_element.SetBackupPosition(newPos);
		_element.SetParentField(this);
		_element.SetPosition(newPos);
		m_elements.push_back(&_element);
	}

	void DropField::DetachElement(DraggableTexture& _element)
	{
		m_elements.erase(std::remove(m_elements.begin(), m_elements.end(), &_element), m_elements.end());
		//Update item positions
		for (int i = 0; i < m_elements.size(); i++)
		{
			//Set backup position to a position inside of the field
			Vec2 newPos = GetScreenSpacePosition() + 0.5f * Vec2(_element.GetSize().x, -_element.GetSize().y);
			//Assumption: All draggable texture have the same width and height
			int maxPerRow = (int)(GetSize().x / _element.GetSize().x);
			int row = (int)(i / maxPerRow);
			int column = i - row * maxPerRow;
			newPos.x += column * _element.GetSize().x;
			newPos.y -= row * _element.GetSize().y;
			m_elements[i]->SetBackupPosition(newPos);
			m_elements[i]->SetPosition(m_elements[i]->GetBackupPosition());
		}
	}

	// ************************************************************************ //
	DraggableTexture::DraggableTexture(const std::string& _name, Vec2 _position, Vec2 _size,
		DefinitionPoint _def, Anchor _anchor, std::vector<DropField*>& _fields, const void* _content) :
		ScreenTexture(_name, _position, _size, _def, _anchor),
		m_content(_content),
		m_fields(_fields)
	{
		m_backupPos = Vec2(0, 0);
		m_fields = _fields;
		m_parentField = nullptr;
	}

	void DraggableTexture::MouseEnter()
	{
		ScreenOverlay::MouseEnter();

		m_state = State::MouseOver;
	}

	void DraggableTexture::MouseLeave()
	{
		ScreenOverlay::MouseLeave();

		m_state = State::Base;
	}

	void DraggableTexture::UpdatePosition()
	{
		Vec2 cursor = Control::InputManager::GetCursorPosScreenSpace();
		SetPosition(cursor + Vec2(GetSize().x * 0.5f, -GetSize().y * 0.5f) + m_offset);
	}

	void DraggableTexture::MouseMove(float _dx, float _dy)
	{
		ScreenOverlay::MouseMove(_dx, _dy);

		if (m_state == State::Pressed)
		{
			m_backupPos = GetPosition();
			m_state = State::Dragged;
			UpdatePosition();
		}
		else if (m_state == State::Dragged)
			UpdatePosition();
	}

	bool DraggableTexture::KeyDown(int _key, int _modifiers, ei::Vec2 _pos)
	{
		ScreenOverlay::KeyDown(_key, _modifiers, _pos);

		if (m_state != State::Dragged)
		{
			m_state = State::Pressed;
			Vec2 cursor = Control::InputManager::GetCursorPosScreenSpace();
			m_offset = GetScreenSpacePosition() - cursor;
		}

		//since an object(this) is hit, the return value is always true
		return true;
	}

	bool DraggableTexture::KeyUp(int _key, int _modifiers, ei::Vec2 _pos)
	{
		ScreenOverlay::KeyUp(_key, _modifiers, _pos);

		if (m_state == State::Dragged)
		{
			//Check if cursor is inside of one of the dropfields
			Vec2 cursor = Control::InputManager::GetCursorPosScreenSpace();
			for (int i = 0; i < m_fields.size(); i++)
			{
				Vec2 pos = m_fields[i]->GetScreenSpacePosition();
				pos.y -= m_fields[i]->GetSize().y;
				if (cursor.x >= pos.x && cursor.x <= pos.x + m_fields[i]->GetSize().x
					&& cursor.y >= pos.y && cursor.y <= pos.y + m_fields[i]->GetSize().y)
				{
					m_fields[i]->DropElement(*this);
				}
			}

			//Drop in underlying drop field or go back to backup position
			SetPosition(m_backupPos);
		}

		m_state = State::MouseOver;

		return true;
	}

	// ************************************************************************ //

	Button::Button(const std::string& _name, Vec2 _position, Vec2 _size,
		DefinitionPoint _def, Anchor _anchor, const std::string& _caption,
		std::function<void()> _OnMouseUp, Font* _font) :
		ScreenTexture(_name + "Default", _position, _size, _def, _anchor, _OnMouseUp),
		m_btnDefault(_name + "Default", _position, _size, _def, _anchor),
		m_btnOver(_name + "Over", _position, _size, _def, _anchor),
		m_btnDown(_name + "Down", _position, _size, _def, _anchor),
		m_caption(Vec2(0.f), Anchor(DefinitionPoint::TopLeft, this), _font),
		m_autoCenter(true)
	{
		SetVisible(false);
		SetActive(true);

		m_btnDefault.SetVisible(true);
		m_btnOver.SetVisible(false);
		m_btnDown.SetVisible(false);

		m_caption.SetDefaultSize(2.f);
		SetCaption(_caption);
	}

	void Button::Register(Hud& _hud)
	{
		_hud.RegisterElement(m_caption);
		m_btnDefault.Register(_hud);
		m_btnOver.Register(_hud);
		m_btnDown.Register(_hud);

		//add the collision overlay last so it is in front of the elements
		ScreenTexture::Register(_hud);

		SetCaption(m_caption.GetText());
	}

	// ************************************************************************ //
	void Button::SetPosition(Vec2 _pos)
	{
		ScreenTexture::SetPosition(_pos);
		/*		m_btnDefault.SetPosition(_pos);
				m_btnOver.SetPosition(_pos);
				m_btnDown.SetPosition(_pos);
				m_caption.SetPosition(_pos);*/
		SetCaption(m_caption.GetText());
	}

	// ************************************************************************ //
	void Button::SetSize(Vec2 _size)
	{
		ScreenOverlay::SetSize(_size);
		m_btnDefault.SetSize(_size);
		m_btnOver.SetSize(_size);
		m_btnDown.SetSize(_size);
	}

	// ************************************************************************ //

	void Button::Scale(Vec2 _scale)
	{
		ScreenTexture::Scale(_scale);
		m_btnDefault.Scale(_scale);
		m_btnOver.Scale(_scale);
		m_btnDown.Scale(_scale);

		SetCaption(m_caption.GetText());
	}

	// ************************************************************************ //
	void Button::SetCaption(const std::string& _caption)
	{
		Vec2 captionDim = m_caption.GetCharSize();

		//set the (new) caption first to rebuild the buffer
		m_caption.SetText(_caption);
		m_caption.SetRectangle(m_size);

		// center in both directions
		Vec2 rect = m_caption.GetRectangle();//captionDim[0] * charCountMax * m_caption.GetDefaultSize()
		m_caption.SetPosition(Vec2(!m_autoCenter[0] ? 0.f : ((m_size[0] - rect.x) * 0.5f),
			!m_autoCenter[1] ? 0.f : (-m_size[1] + rect.y) * 0.5f));//- captionDim[1] * m_caption.GetMaxSize() * 0.45f

	}

	void Button::MouseEnter()
	{
		ScreenOverlay::MouseEnter();
		m_btnDefault.SetVisible(false);
		m_btnOver.SetVisible(true);

		m_btnState = State::MouseOver;
		m_caption.SetDefaultColor(Utils::Color8U((uint8_t)210, 210, 210));
	}
	void Button::MouseLeave()
	{
		ScreenOverlay::MouseLeave();
		m_btnDefault.SetVisible(true);
		m_btnOver.SetVisible(false);
		m_btnDown.SetVisible(false);

		m_btnState = State::Base;
		m_caption.SetDefaultColor(Utils::Color8U((uint8_t)255, 255, 255));
	}
	bool Button::KeyDown(int _key, int _modifiers, Vec2 _pos)
	{
		m_btnDefault.SetVisible(false);
		m_btnOver.SetVisible(false);
		m_btnDown.SetVisible(true);

		m_btnState = State::Pressed;
		//since an object(this) is hit, the return value is always true
		return true;
	}

	bool Button::KeyUp(int _key, int _modifiers, Vec2 _pos)
	{
		if (OnMouseUp) OnMouseUp();

		ScreenOverlay::KeyUp(_key, _modifiers, _pos);
		m_btnDefault.SetVisible(true);
		m_btnOver.SetVisible(false);
		m_btnDown.SetVisible(false);
		m_btnState = State::MouseOver;

		return true;
	}

	// ************************************************************** //

	Indicator::Indicator(Vec2 _position, const Game::Actor& _target, Hud& _hud, const std::string& _tag, Utils::Color8U _color)
		: ScreenTexture("", _position, PixelOffset(32, 32), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, &_hud)),
		m_target(_target),
		m_tag(_tag),
		m_textures{ { "indicator_up", _position, PixelOffset(32, 32), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, &_hud) },
			{ "indicator_down", _position, PixelOffset(32, 32), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, &_hud) },
			{"indicator_left", _position, PixelOffset(32, 32), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, &_hud) },
			{ "indicator_right", _position, PixelOffset(32, 32), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, &_hud) },
			{ "focus_indicator", _position, PixelOffset(64, 64), DefinitionPoint::MidMid, Anchor(DefinitionPoint::MidMid, &_hud) } },
		m_distanceLabel(Vec2(0.f), Anchor(DefinitionPoint::MidMid, this), nullptr, "", 0.5f)
	{
		SetVisible(false);
		for (int i = 0; i < 5; i++)
		{
			m_textures[i].SetVisible(false);
			m_textures[i].SetActive(false);
			m_textures[i].SetColor(_color);
		}
		m_direction = Direction::None;
		m_distanceLabel.SetDefaultColor(_color);
	}

	void Indicator::SetDirection(Direction _direction)
	{
		m_textures[m_direction].SetVisible(false);
		m_textures[_direction].SetVisible(true);
		m_direction = _direction;
		UpdateLabel();
	}

	void Indicator::SetPosition(Vec2 _pos)
	{
		ScreenTexture::SetPosition(_pos);
		for (int i = 0; i < 5; i++)
			m_textures[i].SetPosition(_pos);
		UpdateLabel();
	}

	void Indicator::UpdateLabel()
	{
		switch (m_direction)
		{
		case Direction::Up:
			m_distanceLabel.SetPosition(Vec2(-GetSize().x * 0.5f, -GetSize().y * 0.5f));
			break;
		case Direction::Down:
			m_distanceLabel.SetPosition(Vec2(-GetSize().x * 0.5f, GetSize().y * 1.5f));
			break;
		case Direction::Left:
			m_distanceLabel.SetPosition(Vec2(0, -GetSize().y * 0.5f));
			break;
		case Direction::Right:
			m_distanceLabel.SetPosition(Vec2(-GetSize().x * 1.5f, -GetSize().y * 0.5f));
			break;
		case Direction::None:
			m_distanceLabel.SetPosition(Vec2(0, 0));
			break;
		}

		float dist = len(Control::g_camera.GetPosition() - m_target.GetPosition());
		std::string distString = std::to_string(dist);
		size_t pos = distString.find('.', 0);
		// dist > 999: 0 digits behind point
		if (int(dist) <= 99)
			distString = distString.substr(0, pos + (3 - pos) + 1);
		else
			distString = distString.substr(0, pos);
		m_distanceLabel.SetText(distString + " m\n[" + m_tag + "]");
	}

	void Indicator::Update()
	{
		bool inSight = false;
		//Project enemy ship to screen space
		Vec4 projected = Control::g_camera.GetViewProjection() * Vec4(GetTarget().GetPosition().x, GetTarget().GetPosition().y, GetTarget().GetPosition().z, 1.0f);
		Vec2 projectedPos = Vec2(projected.x / projected.w, projected.y / projected.w);
		if (dot(GetTarget().GetPosition() - Control::g_camera.GetPosition(), Control::g_camera.GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f)) > 0)
		{
			//If ship is on screen, don't show indicator
			if (abs(projectedPos.y) <= 1.0f && abs(projectedPos.x) <= 1.0f)
				inSight = true;
		}
		if (inSight)
		{
			SetPosition(projectedPos);
			SetDirection(Direction::None);
		}
		else
		{
			Direction dir;
			//Calculate the intersections of the vector to the projected position with the borders of the screen
			float factorY = 1.0f / abs(projectedPos.y);
			Vec2 yIntersection = factorY * projectedPos;
			float factorX = 1.0f / abs(projectedPos.x);
			Vec2 xIntersection = factorX * projectedPos;
			//Get closest intersection
			Vec2 pos = len(yIntersection) <= len(xIntersection) ? yIntersection : xIntersection;
			pos = Vec2(pos.x - sign(pos.x) * GetSize().x / 2.0f, pos.y - sign(pos.y) * GetSize().y / 2.0f);
			//Invert position when on the wrong side
			if (dot(Control::g_camera.GetRotationMatrix() * Vec3(0.0f, 0.0f, 1.0f), GetTarget().GetPosition() - Control::g_camera.GetPosition()) < 0.0f)
				pos *= -1;
			//Set the orientation of the indicator
			if (len(yIntersection) <= len(xIntersection))
				dir = pos.y < 0 ? Direction::Down : Direction::Up;
			else
				dir = pos.x < 0 ? Direction::Left : Direction::Right;
			SetDirection(dir);
			SetPosition(pos);
		}
	}

	void Indicator::Register(Hud& _hud)
	{
		_hud.RegisterElement(m_distanceLabel);
		for (int i = 0; i < 5; ++i)
			m_textures[i].Register(_hud);

		ScreenTexture::Register(_hud);
	}

	void Indicator::Unregister(Hud& _hud)
	{
		for (int i = 0; i < 5; i++)
			_hud.UnregisterElement(m_textures[i]);
		_hud.UnregisterElement(m_distanceLabel);
		ScreenTexture::Unregister(_hud);
	}

	// ************************************************************** //

	EditField::EditField(Vec2 _position, Vec2 _size, Font* _font, int _lines, float _fontSize) :
		ScreenTexture("componentBtnDefault", _position, _size),
		m_linesMax(_lines),
		m_font(_font),
		m_fontSize(_fontSize),
		m_textRender(Vec2(0.f), Anchor(), m_font),
		m_content(""),//init with cursor indicator
		m_cursor(0)
	{
		//	m_lines.emplace_back(new TextRender(_font));
		//	SetVisible(false);
		//	SetActive(true);
		m_textRender.SetText("");
		Vec2 dim = m_textRender.GetCharSize();

		const float textToBoxSize = 0.75f;
		//automatic rezising of the text to fit the field
		if (!_fontSize)
		{
			m_fontSize = m_size[1] * textToBoxSize / dim[1];
			m_textRender.SetDefaultSize(m_fontSize);
		}
		m_textRender.SetRectangle(_size);
		//offset of an half char in x direction ;center in y direction
		m_textRender.SetPosition(m_position + Vec2(m_fontSize * m_textRender.GetCharSize()[0] * 0.5f, -m_size[1] * (textToBoxSize) * 0.5f/* * 0.5f /*- m_textRender.GetRectangle()[1] * 0.5f*/));
	}

	void EditField::Register(Hud& _hud)
	{
		ScreenTexture::Register(_hud);
		_hud.RegisterElement(m_textRender);
	}

	// ************************************************************** //

	void EditField::AddLine(int _preLine)
	{
		/*	int size = (int)m_lines.size();
			if(size >= m_linesMax) return;
			//create new line on right pos
			m_lines.insert(m_lines.begin()+_preLine, std::unique_ptr<TextRender>(new TextRender(m_font)));
			//erange texture and TextRenders
			for(int i = 0; i < size; i++)
				m_lines[i]->SetPosition(m_vertex.position+Math::Vec2(0.02f,-0.75f*i));
			*/
	}

	bool EditField::KeyDown(int _key, int _modifiers, Vec2 _pos)
	{
		//mouse click -> set cursor
		if (_key == GLFW_MOUSE_BUTTON_LEFT)
		{
			//	m_cursor = 0;
			return true; //nothing more happens
		}
		//right arrow key -> shift cursor 
		else if (_key == GLFW_KEY_RIGHT && m_cursor < (int)m_content.size())
		{
			//	std::swap(m_content[m_cursor], m_content[m_cursor + 1]);
			m_cursor++;
		}
		//left arrow key 
		else if (_key == GLFW_KEY_LEFT && m_cursor > 0)
		{
			//	std::swap(m_content[m_cursor], m_content[m_cursor - 1]);
			m_cursor--;
		}
		//backspace
		else if (_key == GLFW_KEY_BACKSPACE && m_cursor > 0)
		{
			m_cursor--;
			m_content.erase(m_cursor, 1);
		}
		//del
		else if (_key == GLFW_KEY_DELETE && m_cursor < (int)m_content.size())
		{
			m_content.erase(m_cursor, 1);
		}
		//printable chars
		//check whether an added char would overflow the rectangle; 1.5 to take into account the start offset
		else if (_key >= 32 && _key <= 162 && m_textRender.GetRectangle()[0] + m_textRender.GetCharSize()[0] * m_fontSize * 1.5f < m_size[0])
		{
			//letter A - Z without shift -> lower case
			if (_key >= 65 && _key <= 90 && !(_modifiers & GLFW_MOD_SHIFT))
				_key += 32;
			//insert happens in front of m_cursor
			m_content.insert(m_cursor, 1, _key);
			m_cursor++;
		}
		else return false;

		//update textRender
		std::string str = m_content;
		str.insert(m_cursor, 1, 'I');
		m_textRender.SetText(str);
		//calc pursor pos
		//take dimensions of the first char, as every 
//		m_cursor[0] = _pos[0] / m_font->m_sizeTable[0][0];
	//	m_cursor[1] = _pos[1] / m_fontSize;
		return true;
	}

	bool EditField::KeyUp(int _key, int _modifiers, Vec2 _pos)
	{
		return true;
	}

	// ************************************************************************ //
	FillBar::FillBar(ei::Vec2 _position, ei::Vec2 _size, DefinitionPoint _def,
		Anchor _anchor, bool _growsRight) :
		ScreenOverlay(_position, _size, _def, _anchor),
		m_growsRight(_growsRight),
		m_texture("fillbar", Vec2(0.f), _size, m_growsRight ? DefP::BotLeft : DefP::BotRight, 
			Anchor(m_growsRight ? DefP::BotLeft : DefP::BotRight, this))
	{

	}

	void FillBar::Register(Hud& _hud)
	{
		_hud.RegisterElement(m_texture);
	}

	// ************************************************************************ //

	void FillBar::SetFillLevel(float _level)
	{
		Vec2 vec(_level, 1.f);
		m_texture.SetScale(vec);
		m_texture.SetTextureRect(vec, m_growsRight ? Vec2(0.f) : Vec2(1.f - _level, 0.f));
		//to update texture vertex
		//todo: remove this
		m_texture.Scale(Vec2(1.f));
	}
};