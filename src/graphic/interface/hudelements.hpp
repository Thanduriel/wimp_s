#pragma once

#include "font.hpp"
#include "game.hpp"
#include "screenoverlay.hpp"
#include "graphic/resources.hpp"
#include "gameplay/elements/ship.hpp"

#include <memory>
#include <queue>

namespace Game {
	class Item;
}

namespace Graphic {

	struct TextureVertex
	{
		ei::Vec2 position;	///< Position on the screen in [-1,1]x[-1,1] where (-1,-1) is the lower left corner
		ei::Vec2 screenSize;
		ei::Vec2 texCoord;	///< Texture position (XY = [0,1]x[0,1]) relative to the lower left corner (0,0)
		ei::Vec2 size;		///< Width and height relative to the texture size [0,1]x[0,1]
		Utils::Color8U color;
	};

	/// \brief A 2d screen overlay texture 
	class ScreenTexture : public ScreenOverlay
	{
	public:
		ScreenTexture( const std::string& _name,
			ei::Vec2 _position, ei::Vec2 _size = ei::Vec2(0.f), DefinitionPoint _def = DefinitionPoint::TopLeft,
			Anchor _anchor = Anchor(),
			std::function<void()> _OnMouseUp = [] () {return;});

		void Register(Hud& _hud) override;
		void Unregister(Hud& _hud) override;
		//override to apply vertex changes 
		virtual void SetPosition(ei::Vec2 _pos) override;
		virtual void SetSize(ei::Vec2 _size) override;
		virtual void Scale(ei::Vec2 _scale) override;

		// The color sampled from the texture is multiplied by this color.
		void SetColor(Utils::Color8U _color) { m_vertex.color = _color; }

		/// \brief scales the rectangle of the source texture that is used
		/// \param _scale the new size relative to the initial size
		void SetTextureRect(ei::Vec2 _scale, ei::Vec2 _pos = ei::Vec2(0.f));
	protected:
		ei::Vec2 m_textureSize; ///< base size of the texture rectangle
		ei::Vec2 m_texturePosition; ///< base position of the texture rectangle
	private:
		TextureVertex m_vertex;
		friend class Hud;
	};

	// ************************************************************* //

	class DraggableTexture;

	class DropField : public ScreenTexture
	{
	public:
		typedef std::function<void(DropField&,DraggableTexture&)> DropEventFn;

		DropField(const std::string& _name,
			ei::Vec2 _position, ei::Vec2 _size = ei::Vec2(0.f), DefinitionPoint _def = DefinitionPoint::TopLeft,
			Anchor _anchor = Anchor(),
			DropEventFn _onDrop = [](DropField&,DraggableTexture&) {});

		//override to apply vertex changes
		virtual void MouseEnter() override;
		virtual void MouseLeave() override;

		void DropElement(DraggableTexture& _element);
		void AppendElement(DraggableTexture& _element);
		void DetachElement(DraggableTexture& _element);

		std::vector<DraggableTexture*>& GetElements() { return m_elements; }
		void SetDropEvent(DropEventFn _onDrop) { m_onDrop = _onDrop; }

		enum struct State
		{
			Base,
			MouseOver
		};
	private:
		void UpdatePosition(DraggableTexture& _element, int _index);

		State m_state;
		std::vector<DraggableTexture*> m_elements;
		DropEventFn m_onDrop;
	};

	// ************************************************************* //
	/// \brief A draggable 2d screen overlay texture 
	class DraggableTexture : public ScreenTexture
	{
	public:
		DraggableTexture(const std::string& _name,
			ei::Vec2 _position, ei::Vec2 _size, DefinitionPoint _def,
			Anchor _anchor,
			std::vector<DropField*>& _fields, const void* _content = nullptr);

		//override to apply vertex changes
		virtual void MouseEnter() override;
		virtual void MouseLeave() override;
		virtual void MouseMove(float _dx, float _dy) override;
		virtual bool KeyDown(int _key, int _modifiers, ei::Vec2 _pos) override;
		virtual bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos) override;

		const ei::Vec2 GetBackupPosition() const { return m_backupPos; };
		void SetBackupPosition(const ei::Vec2 _pos) { m_backupPos = _pos; };
		DropField* GetParentField() const { return m_parentField; };
		void SetParentField(DropField* _field) { m_parentField = _field; };

		const void* GetContent() const { return m_content; }
		enum struct State
		{
			Base,
			MouseOver,
			Pressed,
			Dragged
		};
	private:
		State m_state;
		ei::Vec2 m_backupPos;
		void UpdatePosition();
		std::vector<DropField*>& m_fields;
		DropField* m_parentField;
		ei::Vec2 m_offset;

		const void* m_content;
	};

	class ItemIcon : public DraggableTexture
	{
	public:
		ItemIcon(const Game::Item& _item,
			ei::Vec2 _position, ei::Vec2 _size, Graphic::DefinitionPoint _def,
			Anchor _anchor,
			std::vector<DropField*>& _fields, const void* _content = nullptr);

		void Register(Hud& _hud) override;

		const Graphic::ScreenTexture& GetObjectIcon() { return m_objectIcon; }
	private:
		Graphic::ScreenTexture m_objectIcon;
	};

	// ************************************************************* //

	/// \brief A press-able button with mouseover indication
	class Button : public ScreenOverlay
	{
	public:
		/// \brief creates a button
		Button(const std::string& _name, ei::Vec2 _position, ei::Vec2 _size = ei::Vec2(0.f), 
			DefinitionPoint _def = DefinitionPoint::TopLeft,
			Anchor _anchor = Anchor(), const std::string& _caption = "",
			std::function<void()> _OnMouseUp = [] () {return;}, 
			Font* _font = &Graphic::Resources::GetFont(Graphic::Fonts::DEFAULT));

		void SetCaption(const std::string& _caption);

		void Register(Hud& _hud) override;
		virtual void SetSize(ei::Vec2 _size) override;
		virtual void Scale(ei::Vec2 _scale) override;
		void SetVisible(bool _visible) override;

	//	void SetMouseOverColor(Utils::Color8U _color)

		void SetAutoCenterX(bool _autoCenter) {m_autoCenter[0] = _autoCenter;};
		void SetAutoCenterY(bool _autoCenter) {m_autoCenter[1] = _autoCenter;};

		virtual void MouseEnter() override;
		virtual void MouseLeave() override;
		virtual bool KeyDown(int _key, int _modifiers, ei::Vec2 _pos) override;
		virtual bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos) override;

		template<typename Fn>
		void SetOnMouseUp(Fn&& _event) { OnMouseUp = std::forward<Fn>(_event); }

		enum struct State
		{
			Base,
			MouseOver,
			Pressed
		};
		State GetButtonState() const { return m_btnState; }

	private:
		ScreenTexture m_btnDefault;
		ScreenTexture m_btnOver;
		ScreenTexture m_btnDown;
		TextRender m_caption;

		std::function<void()> OnMouseUp;

		ei::Vec<bool,2> m_autoCenter;
		State m_btnState; // 0 - default; 1 - mouseover; 2 - down
	};

	// ************************************************************* //

	enum Direction
	{
		Up,
		Down,
		Left,
		Right,
		None
	};

	class Indicator : public ScreenTexture
	{
	public:
		static constexpr Utils::Color8U DEFAULT_COLOR = Utils::Color8U(68_uc, 109_uc, 178_uc);
		Indicator(ei::Vec2 _position, const Game::Actor& _target, Hud& _hud, const std::string& _tag, Utils::Color8U _color = DEFAULT_COLOR);
		
		void SetPosition(ei::Vec2 _pos) override;
		Direction GetDirection() const { return m_direction; };

		void SetDirection(Direction _direction);
		const Game::Actor& GetTarget() const { return **m_target; };
		bool TargetIsDestroyed() const {return !*m_target; }

		void SetColor(Utils::Color8U _color);

		void Register(Hud& _hud) override;
		void Unregister(Hud& _hud) override;

		void Update();
	private:

		void UpdateLabel();

		Direction m_direction;
		const Game::Actor::ConstHandle m_target;
		ScreenTexture m_textures[5];
		TextRender m_distanceLabel;
		std::string m_tag;
	};

	// ************************************************************* //
	/// \brief A panel which allows text input
	class EditField : public ScreenTexture
	{
	public:
		/// \brief Create a usable EditField on the screen
		/// \details param [in] informations needed for texture and Textrender
		/// _lines The amount of lines the field has; 0 means automatic
		/// _fontSize used font size for the text; 0 means automatic
		EditField(ei::Vec2 _position, ei::Vec2 _size, DefinitionPoint _def, Anchor _anchor,
			Font* _font = &Graphic::Resources::GetFont(Graphic::Fonts::DEFAULT),
			int _lines = 1, float _fontSize = 0);

		void Register(Hud& _hud) override;

		/// \brief Returns the current text the field contains.
		const std::string& GetText() const { return m_content; };
		void SetText(const std::string& _text);

		void Unfocus();
	private:
		/// \brief Adds an line after the specified one when it does not violate m_linesMax 
		void AddLine(int _preLine);

		int m_linesMax;
		Font* m_font;
		float m_fontSize; ///< all lines have the same size
		int m_cursor; /// < index of the cursor char in m_content

		std::string m_content;
		TextRender m_textRender; /// < text is stored in a single textRender
		//std::vector< std::unique_ptr< TextRender > > m_lines; 

		virtual bool KeyDown(int _key, int _modifiers, ei::Vec2 _pos) override;
		virtual bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos) override;

	};

	// ************************************************************* //
	class FillBar : public ScreenOverlay
	{
	public:
		FillBar(ei::Vec2 _position, ei::Vec2 _size, DefinitionPoint _def = DefinitionPoint::TopLeft,
			Anchor _anchor = Anchor(), bool _growsRight = false);
		void Register(Hud& _hud) override;

		/// \brief Sets the inner bar to _level * size
		void SetFillLevel(float _level);

		void SetColor(const Utils::Color8U& _color) { m_texture.SetColor(_color); }
	private:
		const bool m_growsRight;
		ScreenTexture m_texture;
	};

	// ************************************************************* //
	class MessageBox : public ScreenOverlay
	{
	public:
		MessageBox(ei::Vec2 _position, ei::Vec2 _size, DefinitionPoint _def = DefP::TopLeft, Anchor _anchor = Anchor());

		void Register(Hud& _hud) override;


		// special time constants
		constexpr static float AUTO = 0.f;
		constexpr static float INFINITE = -1.f;
		// Add a string to the back of the queue that will be displayed for _time seconds.
		void Push(const std::string& _text, float _time = AUTO);

		void Process(float _deltaTime);

		// should the text be centered?
		void SetCentered(bool _center) { m_isCentered = _center; }
		bool IsCentered() const { return m_isCentered; }
	private:
		void Next(); // advance queue
		std::string CutString(const std::string& _s) const; // adds line breaks so that the text fits the box

		TextRender m_textRender;
		std::queue<std::pair<std::string, float>> m_messages;
		float m_timer;
		bool m_isCentered;
	};

};