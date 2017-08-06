#pragma once

#include "font.hpp"
#include "game.hpp"
#include "screenoverlay.hpp"
#include "graphic/resources.hpp"
#include "gameplay/elements/ship.hpp"

#include <memory>

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
	/// \details To preserve the ratio defined in size one can define _rDim so that the other dimension is scaled to fit the needs
	class ScreenTexture : public ScreenOverlay
	{
	public:
		ScreenTexture( const std::string& _name,
			ei::Vec2 _position, ei::Vec2 _size = ei::Vec2(0.f), DefinitionPoint _def = DefinitionPoint::TopLeft,
			Anchor _anchor = Anchor(),
			std::function<void()> _OnMouseUp = [] () {return;});

		void Register(Hud& _hud) override;
		//override to apply vertex changes 
		virtual void SetPosition(ei::Vec2 _pos) override;
		virtual void SetSize(ei::Vec2 _size) override;
		virtual void Scale(ei::Vec2 _scale) override;

		// The color sampled from the texture is multiplied by this color.
		void SetColor(Utils::Color8U _color) { m_vertex.color = _color; }

		/// \brief scales the rectangle of the source texture that is used
		/// \param _scale the new size relative to the initial size
		void SetTextureRect(ei::Vec2 _scale);
	protected:
		ei::Vec2 m_textureSize; ///< base size of the texture rectangle
	private:
		TextureVertex m_vertex;
		friend class Hud;
	};

	// ************************************************************* //

	class DraggableTexture;

	class DropField : public ScreenTexture
	{
	public:
		DropField(const std::string& _name,
			ei::Vec2 _position, ei::Vec2 _size = ei::Vec2(0.f), DefinitionPoint _def = DefinitionPoint::TopLeft,
			Anchor _anchor = Anchor(),
			std::function<void()> _OnMouseUp = []() {return; });

		//override to apply vertex changes
		virtual void MouseEnter() override;
		virtual void MouseLeave() override;

		void DropElement(DraggableTexture& _element);
		void AppendElement(DraggableTexture& _element);
		void DetachElement(DraggableTexture& _element);

		enum struct State
		{
			Base,
			MouseOver
		};
	private:
		State m_state;
		std::vector<DraggableTexture*> m_elements;
	};

	// ************************************************************* //
	/// \brief A draggable 2d screen overlay texture 
	/// \details To preserve the ratio defined in size one can define _rDim so that the other dimension is scaled to fit the needs
	class DraggableTexture : public ScreenTexture
	{
	public:
		DraggableTexture(const std::string& _name,
			ei::Vec2 _position, ei::Vec2 _size = ei::Vec2(0.f), DefinitionPoint _def = DefinitionPoint::TopLeft,
			Anchor _anchor = Anchor(),
			std::function<void()> _OnMouseUp = []() {return; },
			const std::vector<DropField*>& _fields = std::vector<DropField*>(),
			Hud* _hud = nullptr);

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
		std::vector<DropField*> m_fields;
		DropField* m_parentField;
		Hud* m_hud;
		ei::Vec2 m_offset;
	};

	// ************************************************************* //

	/// \brief A press-able button with mouseover indication
	class Button : public ScreenTexture
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
		virtual void SetPosition(ei::Vec2 _pos) override;
		virtual void SetSize(ei::Vec2 _size) override;
		virtual void Scale(ei::Vec2 _scale) override;

	//	void SetMouseOverColor(Utils::Color8U _color)

		void SetAutoCenterX(bool _autoCenter) {m_autoCenter[0] = _autoCenter;};
		void SetAutoCenterY(bool _autoCenter) {m_autoCenter[1] = _autoCenter;};

		virtual void MouseEnter() override;
		virtual void MouseLeave() override;
		virtual bool KeyDown(int _key, int _modifiers, ei::Vec2 _pos) override;
		virtual bool KeyUp(int _key, int _modifiers, ei::Vec2 _pos) override;

		void SetOnMouseUp(std::function<void()>&& _event) { OnMouseUp = std::move(_event); }

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

	class Indicator : public ScreenTexture
	{
	public:
		Indicator(ei::Vec2 _position, Game::Ship& _ship, Anchor _anchor);

		Game::Ship& GetShip() { return m_ship; };
	private:
		Game::Ship& m_ship;
	};

	// ************************************************************* //
	/// \brief A panel which allows text input
	class EditField : public ScreenTexture
	{
	public:
		/// \brief Create a useable EditField on the screen
		/// \details param [in] informations needed for texture and Textrender
		/// _lines The amount of lines the field has; 0 means automatic
		/// _fontSize used fontsize for the text; 0 means automatic
		EditField(ei::Vec2 _position, ei::Vec2 _size, 
			Font* _font = &Graphic::Resources::GetFont(Graphic::Fonts::DEFAULT),
			int _lines = 1, float _fontSize = 1);

		void Register(Hud& _hud) override;

		/// \brief Returns the current text the field contains.
		const std::string& GetText() const { return m_content; };

		/// \brief Returns the textRender to directly manipulate the text.
		TextRender* getTextRender() { return &m_textRender; };
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
	class FillBar : public ScreenTexture
	{
	public:
		FillBar(ei::Vec2 _position, ei::Vec2 _size, DefinitionPoint _def = DefinitionPoint::TopLeft,
			Anchor _anchor = Anchor());
	//	void Register(Hud& _hud) override;

		/// \brief Sets the inner bar to _level * size
		void SetFillLevel(float _level);
	};

};