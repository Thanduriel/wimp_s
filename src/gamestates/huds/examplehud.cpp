#include "examplehud.hpp"
#include "graphic/interface/pixelcoords.hpp"

namespace GameStates {

	using namespace Graphic;
	using namespace ei;

	ExampleHud::ExampleHud()
	{
		// CREATION
		// All elements should be created with Hud::CreateScreenElement.
		// This function takes the same arguments as the constructor of the specific element
		// and takes care of registering it correctly.
		// POSITION
		// Elements take a position in screen space as Vec2. That is [-1,1]x[-1,1] 
		// with (-1,-1) being the lower left corner of the screen.
		// There are several mechanics to ease positioning elements.
		// The DefinitionPoint sets which local point on the given object should be at the given position.
		// First of PixelCoord can be used, from which the correct position will be calculated
		// based on the current screen size.
		// Even better is to use a relative description.
		// An Anchor describes which point is taken as origin.
		// Here the bottom left point of owning Hud is used.
		// Pay in mind that the position given is then an offset from the Anchor.
		// Thus you should use PixelOffset instead.
		// SIZE
		// Size is given as screen size, ranging in [0,2].
		// Again PixelOffset can be used to describe the size in pixels.
		m_fillBar = &CreateScreenElement<FillBar>(PixelOffset(20, 50), PixelOffset(200, 26), DefP::BotLeft, Anchor(DefP::BotLeft, this));

		// This text render is attached to the fillbar to function as its description.
		m_textRender = &CreateScreenElement<TextRender>(PixelOffset(0, 5), Anchor(DefP::BotLeft, m_fillBar));
		m_textRender->SetDefaultSize(0.7f);
		m_textRender->SetText("energy level");

		// When no size(or Vec2(0.f)) is given the original size of the texture in pixels is used.
		// This is especially useful to preserve the aspect ratio.
		// Here a cross-hair is placed by putting its center into the center of the hud.
		ScreenOverlay* el = &CreateScreenElement<ScreenTexture>("cursorAlt", Vec2(0.f), Vec2(0.f), DefP::MidMid, Anchor(DefP::MidMid, this));
		// The size can still be changed without distorting the texture by scaling.
		el->Scale(Vec2(0.8f));

		// This button is placed in the right corner and the size is automatically deducted.
		m_button = &CreateScreenElement<Graphic::Button>("menuBtn", Vec2(0.f), Vec2(0.f),
			DefinitionPoint::BotRight, ScreenPosition::Anchor(DefinitionPoint::BotRight, this), "click");
		m_button->Scale(Vec2(0.25f));

		// Any function with the signature void() can serve as the invoked action on pressing a button.
		m_button->SetOnMouseUp([&]() 
		{
			static float l = 0.f;
			if (l >= 1.f) l = 0.f;
			l += 0.1f;
			m_fillBar->SetFillLevel(l);
		});
	}
}