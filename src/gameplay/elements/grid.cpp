#include "grid.hpp"
#include "control/camera.hpp"

namespace Game {
	Grid::Grid(const ei::Vec3& _position, const ei::Quaternion& _rotation,
		const Utils::Color32F& _color, float _resolutionX, float _resolutionZ, float _radius )
		: Actor(_position, _rotation),
		m_lineRenderer(_color)
	{
		float ix = -_radius;
		for (; ix < _radius - _resolutionX; ix += _resolutionX)
		{
			float iz = -_radius;
			for (; iz < _radius - _resolutionZ; iz += _resolutionZ)
			{
				m_lineRenderer.Add(ei::Vec3(ix, 0.f, iz), ei::Vec3(ix, 0.f, iz + _resolutionZ));
				m_lineRenderer.Add(ei::Vec3(ix, 0.f, iz), ei::Vec3(ix + _resolutionX, 0.f, iz));
			}
			m_lineRenderer.Add(ei::Vec3(ix, 0.f, iz), ei::Vec3(ix + _resolutionX, 0.f, iz));
		}
		for (float iz = -_radius; iz < _radius - _resolutionZ; iz += _resolutionZ)
			m_lineRenderer.Add(ei::Vec3(ix, 0.f, iz), ei::Vec3(ix, 0.f, iz + _resolutionZ));
	}

	void Grid::Draw()
	{
		m_lineRenderer.Draw(Control::g_camera.GetViewProjection() * m_transformation);
	}
}