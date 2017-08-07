#include <algorithm>

#include "grid.hpp"
#include "control/camera.hpp"
#include "math/extensions.hpp"

namespace Game {

	GridComponent::GridComponent(const Actor& _actor, const Utils::Color32F& _color, TransitionInfo&& _transitionInfo)
		: MarkerComponent(_actor),
		m_lineRenderer(_color),
		m_transitionInfo(std::move(_transitionInfo)),
		m_currentLine(0),
		m_linesShown(0.f),
		m_fadeIn(false)
	{}

	// ******************************************************************* //
	GridComponent::GridComponent(const Actor& _actor, const Utils::Color32F& _color, float _resolutionX, float _resolutionZ, float _radius, TransitionInfo&& _transitionInfo)
		: GridComponent(_actor, _color, std::move(_transitionInfo))
	{
		float ix = -_radius;
		for (; ix < _radius - _resolutionX; ix += _resolutionX)
		{
			float iz = -_radius;
			for (; iz < _radius - _resolutionZ; iz += _resolutionZ)
			{
				m_lines.emplace_back(ei::Vec3(ix, 0.f, iz), ei::Vec3(ix, 0.f, iz + _resolutionZ));
				m_lines.emplace_back(ei::Vec3(ix, 0.f, iz), ei::Vec3(ix + _resolutionX, 0.f, iz));
			}
			m_lines.emplace_back(ei::Vec3(ix, 0.f, iz), ei::Vec3(ix + _resolutionX, 0.f, iz));
		}
		for (float iz = -_radius; iz < _radius - _resolutionZ; iz += _resolutionZ)
			m_lines.emplace_back(ei::Vec3(ix, 0.f, iz), ei::Vec3(ix, 0.f, iz + _resolutionZ));

		if (m_transitionInfo.speed != 0.f)
			std::sort(m_lines.begin(), m_lines.end(), m_transitionInfo.comparisonFunction);

		for (auto& line : m_lines)
			m_lineRenderer.Add(line.first, line.second);
	}

	// ******************************************************************* //
	GridComponent::GridComponent(const Actor& _actor, const Utils::Color32F& _color, int _numLines, float _radius)
		: GridComponent(_actor, _color, TransitionInfo{})
	{
		AddCircle(_numLines, _radius);
	}

	// ******************************************************************* //
	GridComponent::GridComponent(const Actor& _actor, const Utils::Color32F& _color, int _numLinesCircle,
		int _numCircles, float _radius)
		: GridComponent(_actor, _color, TransitionInfo{})
	{
		float lineDist = 2.f * _radius / _numCircles;
		float rSqr = _radius * _radius;

		// start at half interval because a circle with radius zero is hardly visible
		float y = -_radius + lineDist * 0.5f;

		ei::Mat4x4 rot = ei::Mat4x4(ei::rotationX(ei::PI * 0.5f));

		for (int i = 0; i < _numCircles; ++i)
		{
			float r = sqrt(rSqr - y*y);
			ei::Mat4x4 move = ei::translation(ei::Vec3(0.f, y, 0.f));

			AddCircle(_numLinesCircle, r, move);
			AddCircle(_numLinesCircle, r, rot * move);
			y += lineDist;
		}
	}

	// ******************************************************************* //
	void GridComponent::ProcessComponent(float _deltaTime)
	{
		if (m_transitionInfo.speed == 0.f) return;

		float fact = m_linesShown / (float)m_lines.size();
		fact = ei::lerp(m_transitionInfo.speedModifier, 1.f / m_transitionInfo.speedModifier, fact);

		if (m_fadeIn)
		{
			m_linesShown += _deltaTime * m_transitionInfo.speed * fact;
			m_linesShown = std::min(m_linesShown, (float)m_lines.size() - 1.f);
		}
		else
		{
			m_linesShown -= _deltaTime * m_transitionInfo.speed * fact;
			m_linesShown = std::max(m_linesShown, 0.f);
		}
		m_lineRenderer.SetLinesShown((int)m_linesShown);
	}
	
	void GridComponent::Draw()
	{
		m_lineRenderer.Draw(Control::g_camera.GetViewProjection() * m_actor.GetTransformation());
	}

	void GridComponent::AddCircle(int _numSegments, float _radius, const ei::Mat4x4& _transform)
	{
		float rad = 2.f * ei::PI / _numSegments;
		float end = rad;

		ei::Vec3 beginV = ei::Vec3(sin(0) * _radius, 0.f, cos(0) * _radius);
		ei::Vec3 endV = ei::Vec3(sin(end) * _radius, 0.f, cos(end) * _radius);

		for (int i = 0; i < _numSegments; ++i)
		{
			m_lineRenderer.Add(_transform * beginV, _transform * endV);
			beginV = endV;
			end += rad;
			endV = ei::Vec3(sin(end) * _radius, 0.f, cos(end) * _radius);
		}
	}

	bool GridComponent::closerToCenter(const std::pair<ei::Vec3,ei::Vec3>& _lhs, const std::pair<ei::Vec3, ei::Vec3>& _rhs)
	{
		return ei::dot(_lhs.first, _lhs.first) < ei::dot(_rhs.first, _rhs.first);
	}
}