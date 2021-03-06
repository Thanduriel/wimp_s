#pragma once

#include "gameplay/core/component.hpp"
#include "graphic/mesh.hpp"

namespace Game {

	// A visual effect applied on top of a mesh.
	class ShieldComponent : public ConstActorComponent
	{
	public:
		ShieldComponent(const Actor& _actor, const Graphic::Mesh& _mesh);

		void ProcessComponent(float _deltaTime) override;

		void Draw();

		void SetIntensity(float _intensity) { m_intensity = _intensity; }
	private:
		float m_localTime;
		float m_maxTime;
		float m_size;
		float m_speedMod;
		float m_intensity;
		const Graphic::Mesh& m_mesh;
	};
}