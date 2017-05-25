#include "starbackground.hpp"
#include "graphic/resources.hpp"
#include "generators/random.hpp"
#include "graphic/core/device.hpp"
#include "graphic/core/effect.hpp"
#include "ei/vector.hpp"
#include "utils/color.hpp"


using namespace ei;

namespace Graphic {

	const int c_minTemperature = 2000u;
	const int c_maxTemperature = 40000u;

	Starbackground::Starbackground(int _stars, float _size, int _ambientStars) :
		m_ambientStars(Graphic::VertexArrayBuffer::PrimitiveType::POINT, { {Graphic::VertexAttribute::VEC3, 0}, {Graphic::VertexAttribute::COLOR, 3}, {Graphic::VertexAttribute::FLOAT, 11} })
	{
		auto vbGuard = m_ambientStars.GetBuffer(0);

		//generator params
		//stars in the core blob / stars in the spiral arms
		float coreToRemoteRatio = 0.40f;
		//rotation of the arms in complete circles
		float rotationRate = 2.25f;

		float radius = _size / 2;
		float radiusSq = radius * radius;
		float height = radius / 32;
		float coreStars = coreToRemoteRatio * _stars;
		float factor = 0.99f / powf(_stars - coreStars, 2.f);

		Vec3 pos;

		Generators::DefaultRandomGen rnd((uint32_t)_stars);
		Generators::RandomSampler<Generators::DefaultRandomGen> sampler(rnd);
		for (int i = 0; i < _stars; ++i)
		{
			//center is a cloud
			if (i / (float)_stars < coreToRemoteRatio)
			{
				std::normal_distribution< double > normalDist(0.f, radius * 8.f);
				pos[0] = (float)normalDist(rnd);
				pos[1] = (float)normalDist(rnd);
				pos[2] = (float)normalDist(rnd);
			}
			else
			{
				//spiral arms start in the core
				float c = i - coreStars;
				//radius function
				//Dr = [coreStars,_stars - coreStars]
				//Wr = [0,1]
				// y = a(x-d)^2
				//float f = c / float(100 + c);
				//float r = c / float(_stars - coreStars);
				//non linear so that the density of stars decreases with growing radius
				float r = factor*pow(c, 2.f) + 0.01f;

				//noise decreases linear with radius
				float relative = c / (float)(_stars - coreStars);
				float f2 = (1.2f - relative) * radius * 2;
				//2 arms starting on the same axis (+-)
				pos[0] = (r*radius + sampler.Normal(f2)) *(i % 2 ? 1.f : -1.f);
				pos[2] = 0.f;
				//rotation is linear
				float a = relative * 2 * PI * rotationRate + sampler.Exponential(relative);
				pos = rotationY(a) * pos;

				pos[1] = sampler.Uniform(-height, height);
			}

			StarVertex vertex{ pos,
				TemperatureToRGB(sampler.Uniform(c_minTemperature, c_maxTemperature)).RGBA(),
				sampler.Uniform(1.0f, 2.5f) };
			vbGuard->Add(vertex);
		}

		for (int i = 0; i < _ambientStars; ++i)
		{
			StarVertex star;
			star.color = TemperatureToRGB(sampler.Uniform(c_minTemperature, c_maxTemperature)).RGBA();
			star.color -= (sampler.Uniform(-32, 32) + 115) << 24; // background stars have less brightness
			star.position = sampler.Direction() * 10.0f;
			star.size = sampler.Uniform(1.0f, 2.2f);

			vbGuard->Add(star);
		}
	}

	void Starbackground::Draw()
	{

		Graphic::Effect& effect = Graphic::Resources::GetEffect(Graphic::Effects::BACKGROUNDSTAR);
		Graphic::Device::SetEffect(effect);
		Graphic::Device::DrawVertices(m_ambientStars, 0, m_ambientStars.GetNumVertices());
	}

	Utils::Color8U Starbackground::TemperatureToRGB(int _temperature)
	{
		//all calculations need this
		_temperature /= 100;

		int red;
		int green;
		int blue;

		//red
		if (_temperature <= 66)
			red = 255;
		else
		{
			red = _temperature - 60;
			red = int(329.698727446 * pow(red, -0.1332047592));
			if (red < 0)  red = 0;
			if (red > 255)  red = 255;
		}

		//green
		if (_temperature <= 66)
		{
			green = _temperature;
			green = int(99.4708025861 * log2(green) - 161.1195681661);
		}
		else
		{
			green = _temperature - 60;
			green = int(288.1221695283 * pow(green, -0.0755148492));
		}
		//check boundries
		if (green < 0)  green = 0;
		else if (green > 255) green = 255;

		//blue
		if (_temperature >= 66)
			blue = 255;
		else if (_temperature <= 19)
			blue = 0;
		else
		{
			blue = _temperature - 10;
			blue = int(138.5177312231 * log2(blue) - 305.0447927307);
			if (blue < 0) blue = 0;
			if (blue > 255) blue = 255;
		}

		return Utils::Color8U((uint8)red, (uint8)green, (uint8)blue);

	}

}