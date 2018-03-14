#include "settingshud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "graphic/interface/customelements.hpp"

namespace GameStates {

	using namespace Graphic;
	using namespace ei;

	SettingsHud::SettingsHud()
	{
		m_applyButton = &CreateScreenElement<MenuButton>("menuBtn", PixelOffset(-10,0), Vec2(0.f), DefinitionPoint::BotRight, Anchor(DefinitionPoint::BotMid, this), "Apply");
		m_applyButton->Scale(Vec2(0.5f, 0.5f));
		m_cancelButton = &CreateScreenElement<MenuButton>("menuBtn", PixelOffset(10, 0), Vec2(0.f), DefinitionPoint::BotLeft, Anchor(DefinitionPoint::BotMid, this), "Cancel");
		m_cancelButton->Scale(Vec2(0.5f, 0.5f));

		m_aimAssistButton = &CreateScreenElement<MenuButton>("menuBtn", PixelOffset(0, 300), Vec2(0.f), DefinitionPoint::BotMid, Anchor(DefinitionPoint::MidMid, this), "aim assist: OFF");
		m_aimAssistButton->Scale(Vec2(0.5f, 0.5f));
		m_mouseSensitivity = &CreateScreenElement<EditField>(PixelOffset(0, -20), PixelOffset(200, 50), DefP::TopLeft, Anchor(DefP::BotMid, m_aimAssistButton));
		TextRender& label = CreateScreenElement<TextRender>(Vec2(0.f), Anchor(DefP::MidLeft, m_mouseSensitivity), nullptr, "mouse sensitivity: ", DefP::MidRight);
		label.SetDefaultSize(0.6f);
	//	label.SetPosition(Vec2(-label.GetRectangle().x, 0.f));

		m_resolutionButton = &CreateScreenElement<MenuButton>("menuBtn", PixelOffset(0, -40), Vec2(0.f), DefinitionPoint::TopMid, Anchor(DefinitionPoint::BotLeft, m_mouseSensitivity), "1920x1080");
		m_resolutionButton->Scale(Vec2(0.5f, 0.5f));
		m_fullScreenButton = &CreateScreenElement<MenuButton>("menuBtn", PixelOffset(0, -20), Vec2(0.f), DefinitionPoint::TopMid, Anchor(DefinitionPoint::BotMid, m_resolutionButton), "fullscreen: OFF");
		m_fullScreenButton->Scale(Vec2(0.5f, 0.5f));
		m_frameCountTarget = &CreateScreenElement<EditField>(PixelOffset(0, -20), PixelOffset(200, 50), DefP::TopLeft, Anchor(DefP::BotMid, m_fullScreenButton));
		TextRender& label2 = CreateScreenElement<TextRender>(Vec2(0.f), Anchor(DefP::MidLeft, m_frameCountTarget), nullptr, "target frame rate: ", DefP::MidRight);
		label2.SetDefaultSize(0.6f);
	//	label2.SetPosition(Vec2(-label2.GetRectangle().x, 0.f));

		TextRender& note = CreateScreenElement<TextRender>(Vec2(0.f), Anchor(DefP::TopLeft, m_applyButton), nullptr, "|changes to video settings require a restart", DefP::BotLeft);
		note.SetDefaultSize(18_px);
	}
}