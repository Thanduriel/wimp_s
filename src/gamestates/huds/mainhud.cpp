#include "mainhud.hpp"
#include "graphic/interface/pixelcoords.hpp"
#include "control/camera.hpp"
#include "ei/3dintersection.hpp"
#include <string.h>

namespace GameStates {

	using namespace Graphic;
	using namespace ei;

	MainHud::MainHud()
	{
		ShowCursor(Hud::CursorType::Crosshair);

		float outerSpeedBarHeight = 512;
		m_outerSpeedBar = &CreateScreenElement<ScreenTexture>("speed_bar_outer", PixelOffset(10, 10), PixelOffset(64, outerSpeedBarHeight), DefP::BotLeft, Anchor(DefP::BotLeft, this));
		m_innerSpeedBar = &CreateScreenElement<ScreenTexture>("speed_bar_inner", PixelOffset(10, 10), PixelOffset(64, 512), DefP::BotLeft, Anchor(DefP::BotLeft, this));
		m_speedLabel = &CreateScreenElement<TextRender>(PixelOffset(0, 50), Anchor(DefP::BotLeft, m_outerSpeedBar));
		m_speedLabel->SetText("0 m/s");
		m_speedLabel->SetDefaultSize(0.7f);
		m_speedSlider = &CreateScreenElement<ScreenTexture>("targetspeed_slider", Vec2(0, 0.11328125f * m_outerSpeedBar->GetSize().y), PixelOffset(128, 32), DefP::MidMid, Anchor(DefP::BotMid, m_outerSpeedBar));
		m_crossHairDot = &CreateScreenElement<ScreenTexture>("crosshair_dot", PixelOffset(0, 0), PixelOffset(32, 32), DefP::MidMid, Anchor(DefP::MidMid, this));
		m_crossHairLeft = &CreateScreenElement<ScreenTexture>("crosshair_left", PixelOffset(0, 0), PixelOffset(64, 64), DefP::MidRight, Anchor(DefP::MidMid, m_crossHairDot));
		m_crossHairRight = &CreateScreenElement<ScreenTexture>("crosshair_right", PixelOffset(0, 0), PixelOffset(64, 64), DefP::MidLeft, Anchor(DefP::MidMid, m_crossHairDot));

		// todo: different texture for this
		m_specialMoveIndicator = &CreateScreenElement<ScreenTexture>("targetspeed_slider", PixelOffset(0, 20), Vec2(0.f), DefP::BotMid, Anchor(DefP::BotMid, this));
		m_specialMoveIndicator->SetColor(Utils::Color8U(250_uc, 32_uc, 255_uc));
		m_healthBar = &CreateScreenElement<FillBar>(PixelOffset(-36, 20), PixelOffset(220, 28), DefP::BotRight, Anchor(DefP::BotRight, this));
		m_healthBar->SetColor(Utils::Color8U(173_uc, 226_uc, 70_uc));
		m_energyBar = &CreateScreenElement<FillBar>(PixelOffset(0, 10), PixelOffset(220, 28), DefP::BotRight, Anchor(DefP::TopRight, m_healthBar));
	}

	void MainHud::UpdateSpeedLabel(float _speed)
	{
		std::string speedString = std::to_string(_speed);
		size_t pos = speedString.find('.', 0);
		// +2 for 1 character after point
		speedString = speedString.substr(0, pos + 2);
		//We only move between speeds of 0 - 999
		for (int i = 0; i < 3 - (int)pos; i++)
			speedString = " " + speedString;
		m_speedLabel->SetText(speedString + " m/s");
	}

	void MainHud::UpdateSpeedBar(float _speed, float _maxSpeed)
	{
		//Magic numbers FTW!!!
		//The relative heights from the bottom of the drawn texture to the top of the drawn texture
		Vec2 vec(1.0f, 0.11328125f + 0.7734375f * (_speed / _maxSpeed));
		if (_speed >= _maxSpeed - 0.01f)
			vec[1] = 1.0f;
		m_innerSpeedBar->SetScale(vec);
		m_innerSpeedBar->SetTextureRect(vec);
		m_innerSpeedBar->Scale(Vec2(1.f));
	}

	void MainHud::UpdateTargetSpeedSlider(float _targetSpeed, float _maxSpeed)
	{
		float height = 0.11328125f + 0.7734375f * (_targetSpeed / _maxSpeed);
		m_speedSlider->SetPosition(Vec2(0, height * m_outerSpeedBar->GetSize().y));
	}

	void MainHud::UpdateCrossHair(float _sprayRadius)
	{
		float maxSpread = 50.0f;
		float spread = maxSpread * _sprayRadius;
		m_crossHairLeft->SetPosition(PixelOffset(-spread, 0));
		m_crossHairRight->SetPosition(PixelOffset(spread, 0));
	}

	enum ScreenBorder
	{
		Top,
		Bottom,
		Left,
		Right
	};

	void MainHud::UpdateIndicators(Vec3 _playerPos)
	{
		//Get the normals of the 4 planes limiting the view frustum (near and far plane excluded)
		Vec3 v1 = Control::g_camera.GetRotationMatrix() *  Vec3(0, -1.0f, 0);
		Vec3 perp1 = Control::g_camera.GetRotationMatrix() * Vec3(-1.0f, 0, 0);
		Vec3 normalDown = ei::rotation(perp1, Control::g_camera.GetFov() / 2.0f) * v1;
		Vec3 v2 = Control::g_camera.GetRotationMatrix() *  Vec3(0, 1.0f, 0);
		Vec3 perp2 = Control::g_camera.GetRotationMatrix() * Vec3(1.0f, 0, 0);
		Vec3 normalUp = ei::rotation(perp2, Control::g_camera.GetFov() / 2.0f) * v2;
		float horizontalFov = Control::g_camera.GetFov() * Control::g_camera.GetAspectRatio();
		Vec3 v3 = Control::g_camera.GetRotationMatrix() *  Vec3(1.0f, 0, 0);
		Vec3 perp3 = Control::g_camera.GetRotationMatrix() * Vec3(0, -1.0f, 0);
		Vec3 normalRight = ei::rotation(perp3, horizontalFov / 2.0f) * v3;
		Vec3 v4 = Control::g_camera.GetRotationMatrix() *  Vec3(-1.0f, 0, 0);
		Vec3 perp4 = Control::g_camera.GetRotationMatrix() * Vec3(0, 1.0f, 0);
		Vec3 normalLeft = ei::rotation(perp4, horizontalFov / 2.0f) * v4;

		for (auto& i : m_indicators)
		{
			//Calculate the distance of the ships to the planes
			Vec3 pos = i->GetShip().GetPosition() - Control::g_camera.GetPosition();
			float dTop = dot(normalDown, pos);
			float dBot = dot(normalUp, pos);
			float dLeft = dot(normalRight, pos);
			float dRight = dot(normalLeft, pos);
			Vec3 delta = i->GetShip().GetPosition() - _playerPos;
			std::vector<std::tuple<float, Vec3, ScreenBorder>> intersectedPlanes;
			//If ship is outside of plane, then calculate intersection point
			if (dTop < 0.0f)
			{
				Vec3 dist = dTop * normalDown;
				Vec3 x = (pow(len(dist), 3) * len(delta) / (dot(dist, delta))) * delta;
				Vec3 p = i->GetShip().GetPosition() - x;
				float d = len(p - _playerPos);
				intersectedPlanes.push_back(std::make_tuple(d, p, ScreenBorder::Top));
			}
			if (dBot < 0.0f)
			{
				Vec3 dist = dBot * normalUp;
				Vec3 x = (pow(len(dist), 3) * len(delta) / (dot(dist, delta))) * delta;
				Vec3 p = i->GetShip().GetPosition() - x;
				float d = len(p - _playerPos);
				intersectedPlanes.push_back(std::make_tuple(d, p, ScreenBorder::Bottom));
			}
			if (dRight < 0.0f)
			{
				Vec3 dist = dRight * normalLeft;
				Vec3 x = (pow(len(dist), 3) * len(delta) / (dot(dist, delta))) * delta;
				Vec3 p = i->GetShip().GetPosition() - x;
				float d = len(p - _playerPos);
				intersectedPlanes.push_back(std::make_tuple(d, p, ScreenBorder::Right));
			}
			if (dLeft < 0.0f)
			{
				Vec3 dist = dLeft * normalRight;
				Vec3 x = (pow(len(dist), 3) * len(delta) / (dot(dist, delta))) * delta;
				Vec3 p = i->GetShip().GetPosition() - x;
				float d = len(p - _playerPos);
				intersectedPlanes.push_back(std::make_tuple(d, p, ScreenBorder::Left));
			}

			if (intersectedPlanes.size() > 0)
			{
				std::vector<std::tuple<float, Vec3, ScreenBorder>>::iterator smallest = std::min_element(intersectedPlanes.begin(), intersectedPlanes.end(), [](auto& lhs, auto& rhs)
				{
					return std::get<0>(lhs) < std::get<0>(rhs);
				});
				std::tuple<float, Vec3, ScreenBorder> element = intersectedPlanes[std::distance(std::begin(intersectedPlanes), smallest)];
				Vec4 projected = Control::g_camera.GetViewProjection() * Vec4(std::get<1>(element));
				Vec2 pos = Vec2(ei::clamp(projected.x / projected.w, -1.0f + i->GetSize().x / 2.0f, 1.0f - i->GetSize().x / 2.0f), ei::clamp(projected.y / projected.w, -1.0f + i->GetSize().y / 2.0f, 1.0f - i->GetSize().y / 2.0f));
				if (abs(pos.x) < 1.0f - i->GetSize().x / 2.0f && abs(pos.y) < 1.0f - i->GetSize().y / 2.0f)
				{
					switch (std::get<2>(element))
					{
					case Direction::Up:
						pos.y = 1.0f - i->GetSize().y / 2.0f;
						break;
					case Direction::Down:
						pos.y = -1.0f + i->GetSize().y / 2.0f;
						break;
					case Direction::Left:
						pos.x = -1.0f + i->GetSize().x / 2.0f;
						break;
					case Direction::Right:
						pos.x = 1.0f - i->GetSize().x / 2.0f;
						break;
					}
				}
				i->SetDirection((Direction)(int)std::get<2>(element));
				i->SetPosition(pos);
			}
			else
				i->SetDirection(Direction::None);
		}
	}

	void MainHud::AddIndicator(Game::Ship& _ship)
	{
		m_indicators.push_back(&CreateScreenElement<Indicator>(PixelCoord(0.0f, 0.0f), _ship, *this, Anchor(DefinitionPoint::MidMid, this)));
	}
}