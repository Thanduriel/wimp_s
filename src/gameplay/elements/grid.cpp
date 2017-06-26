#include <algorithm>

#include "grid.hpp"
#include "control/camera.hpp"

namespace Game {
	GridComponent::GridComponent(Actor& _actor, const Utils::Color32F& _color, float _resolutionX, float _resolutionZ, float _radius, TransitionInfo&& _transitionInfo)
		: MarkerComponent(_actor),
		m_lineRenderer(_color),
		m_transitionInfo(std::move(_transitionInfo)),
		m_currentLine(0),
		m_linesShown(0.f),
		m_fadeIn(false)
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

		if (m_transitionInfo.speed == 0.f)
		{
			for (auto& line : m_lines)
				m_lineRenderer.Add(line.first, line.second);
		}
		else
			std::sort(m_lines.begin(), m_lines.end(), m_transitionInfo.comparisonFunction);
	}

	void GridComponent::ProcessComponent(float _deltaTime)
	{
		if (m_transitionInfo.speed == 0.f) return;

		float fact = m_linesShown / (float)m_lines.size();
		fact = ei::lerp(m_transitionInfo.speedModifier, 1.f / m_transitionInfo.speedModifier, fact);

		if (m_fadeIn)
		{
			m_linesShown += _deltaTime * m_transitionInfo.speed * fact;

			size_t newLine = (size_t)m_linesShown;
			newLine = std::min(newLine, m_lines.size() - 1);
			for (; m_currentLine < newLine; ++m_currentLine)
				m_lineRenderer.Add(m_lines[m_currentLine].first, m_lines[m_currentLine].second);
		}
		else
		{
			m_linesShown -= _deltaTime * m_transitionInfo.speed * fact;

			size_t newLine = (size_t)m_linesShown;
			newLine = std::max(newLine, (size_t)0);
			for (; m_currentLine > newLine; --m_currentLine)
				m_lineRenderer.Remove();
		}
	}
	
	void GridComponent::Draw()
	{
		m_lineRenderer.Draw(Control::g_camera.GetViewProjection() * m_actor.GetTransformation());
	}

	bool GridComponent::closerToCenter(const std::pair<ei::Vec3,ei::Vec3>& _lhs, const std::pair<ei::Vec3, ei::Vec3>& _rhs)
	{
		return ei::dot(_lhs.first, _lhs.first) < ei::dot(_rhs.first, _rhs.first);
	}
}