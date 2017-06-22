#pragma once

#include <functional>

#include "graphic/effects/linerenderer.hpp"
#include "gameplay/core/actor.hpp"
#include "component.hpp"
#include "utils/color.hpp"

namespace Game {


	class GridComponent : public MarkerComponent
	{
	public:
		typedef std::function<bool(const std::pair<ei::Vec3, ei::Vec3>&, 
			const std::pair<ei::Vec3, ei::Vec3>&)> ComparisonFunc;

		// Descriptor of how the lines should be added.
		struct TransitionInfo
		{
			TransitionInfo() = default;
			// @param _speed add lines in [lines per second]
			// @param _mod the speed is scaled from _mod and linearly
			//		interpolated so that the total time required stays the same.
			// @param _func The function used to order the lines.
			TransitionInfo(float _speed, float _mod = 1.f, ComparisonFunc&& _func = &closerToCenter)
				: speed(_speed), comparisonFunction(std::move(_func)), speedModifier(_mod) {}

			float speed; // in lines per second
			float speedModifier; // acceleration / slow dependend on progress
			ComparisonFunc comparisonFunction;
		};

		GridComponent(Actor& _actor, const Utils::Color32F& _color,
			float _resolutionX = 1.f, float _resolutionZ = 1.f,
			float _radius = 50.f,
			TransitionInfo&& _transitionInfo = TransitionInfo{});

		void ProcessComponent(float _deltaTime) override;
		void Draw() override;

		// Reverses the order of the current transition.
		// Switches between adding and reomving lines.
		void ReverseTransition() { m_fadeIn = !m_fadeIn; m_linesShown = (float)m_currentLine; }

	private:
		static bool closerToCenter(const std::pair<ei::Vec3, ei::Vec3>&,
			const std::pair<ei::Vec3, ei::Vec3>&);

		Graphic::LineRenderer m_lineRenderer;
		
		std::vector<std::pair<ei::Vec3, ei::Vec3>> m_lines;
		TransitionInfo m_transitionInfo;
		size_t m_currentLine;
		float m_linesShown;
		bool m_fadeIn; // should lines be added or removed
	};

	class Grid : public Actor, public GridComponent
	{
	public:
		// Pass the transitionInfo as rvalue if copying the function is expensive.
		Grid(const ei::Vec3& _position, const ei::Quaternion& _rotation,
			const Utils::Color32F& _color,
			float _resolutionX = 1.f, float _resolutionZ = 1.f,
			float _radius = 50.f,
			TransitionInfo&& _transitionInfo = TransitionInfo{});
	};
}