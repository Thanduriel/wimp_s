#pragma once

#include <functional>

#include "graphic/effects/linerenderer.hpp"
#include "gameplay/core/singlecomponentactor.hpp"
#include "utils/color.hpp"

namespace Game {

	// A set of lines in 3d space that share the same transformation.
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
			TransitionInfo(float _speed, float _mod = 1.f, ComparisonFunc&& _func = &CloserToCenter)
				: speed(_speed), comparisonFunction(std::move(_func)), speedModifier(_mod) {}

			float speed; // in lines per second
			float speedModifier; // acceleration / slow depended on progress
			ComparisonFunc comparisonFunction;
		};

		// some comparison functions
		static bool CloserToCenter(const std::pair<ei::Vec3, ei::Vec3>&,
			const std::pair<ei::Vec3, ei::Vec3>&);
		// unique order for different grids
		static bool Random(const std::pair<ei::Vec3, ei::Vec3>&,
			const std::pair<ei::Vec3, ei::Vec3>&);

		// Create a 2d grid on the xz plane with given size and resolution.
		// Pass the transitionInfo as rvalue if copying the function is expensive.
		GridComponent(const Actor& _actor, const Utils::Color32F& _color,
			float _resolutionX = 1.f, float _resolutionZ = 1.f,
			float _radius = 50.f,
			TransitionInfo&& _transitionInfo = TransitionInfo{});

		// Create a circle.
		// @param _numLines The number of lines that form one circle.
		GridComponent(const Actor& _actor, const Utils::Color32F& _color, int _numLines,
			float _radius);

		// Create a sphere from circles.
		// @param _numCircles The number of circles in one direction.
		GridComponent(const Actor& _actor, const Utils::Color32F& _color, int _numLinesCircle,
			int _numCircles, float _radius);

		void ProcessComponent(float _deltaTime) override;
		void Draw() override;

		// Reverses the order of the current transition.
		// Switches between adding and removing lines.
		void ReverseTransition() { m_fadeIn = !m_fadeIn; }

		void SetColor(const Utils::Color32F& _color) { m_lineRenderer.SetColor(_color); }

	protected:
		GridComponent(const Actor& _actor, const Utils::Color32F& _color, TransitionInfo&& _transitionInfo = TransitionInfo{});

		void AddCircle(int _numSegments, float _radius, const ei::Mat4x4& _transform = ei::identity4x4());


		Graphic::LineRenderer m_lineRenderer;
		
		std::vector<std::pair<ei::Vec3, ei::Vec3>> m_lines;
		TransitionInfo m_transitionInfo;
		size_t m_currentLine;
		float m_linesShown;
		bool m_fadeIn; // should lines be added or removed
	};
	typedef SingleComponentActor<GridComponent> Grid;
}