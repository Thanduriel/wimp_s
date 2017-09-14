#include "settingshud.hpp"
#include "graphic/interface/pixelcoords.hpp"

namespace GameStates {

	using namespace Graphic;
	using namespace ei;

	SettingsHud::SettingsHud()
	{
		m_applyButton = &CreateScreenElement<Button>("menuBtn", PixelOffset(-10,0), Vec2(0.f), DefinitionPoint::BotRight, Anchor(DefinitionPoint::BotMid, this), "Apply");
		m_applyButton->Scale(Vec2(0.5f, 0.5f));
		m_cancelButton = &CreateScreenElement<Button>("menuBtn", PixelOffset(10, 0), Vec2(0.f), DefinitionPoint::BotLeft, Anchor(DefinitionPoint::BotMid, this), "Cancel");
		m_cancelButton->Scale(Vec2(0.5f, 0.5f));

		m_aimAssistButton = &CreateScreenElement<Button>("menuBtn", PixelOffset(0, 0), Vec2(0.f), DefinitionPoint::BotMid, Anchor(DefinitionPoint::MidMid, this), "aim assist: OFF");
		m_aimAssistButton->Scale(Vec2(0.5f, 0.5f));
		m_mouseSensitivity = &CreateScreenElement<EditField>(PixelOffset(0, -20), PixelOffset(200, 50), DefP::TopLeft, Anchor(DefP::BotMid, m_aimAssistButton));
		TextRender& label = CreateScreenElement<TextRender>(Vec2(0.f), Anchor(DefP::TopLeft, m_mouseSensitivity), nullptr, "mouse sensitivity: ");
		label.SetDefaultSize(0.6f);
		label.SetPosition(Vec2(-label.GetRectangle().x, 0.f));
	}
}