#pragma once

#include <unordered_map>

#include "utils/color.hpp"
#include "graphic/core/vertexbuffer.hpp"
#include "ei/vector.hpp"

namespace Graphic {
	struct LightInfo
	{
		LightInfo() = default;
		LightInfo(float _radius, Utils::Color8U _color, float _intensity = 1.f);

		ei::Vec3 position; //< position in world space
		float radius;
		Utils::Color8U color;
		float intensity;
	};

	namespace Details {
		typedef uint32_t LightKey;
		typedef std::unordered_map<LightKey, LightInfo> LightInfoContainer;
	}

	class LightHandle
	{
	public:
		LightInfo& operator*();
		const LightInfo& operator*() const;
		LightInfo* operator->();
		const LightInfo* operator->() const;
	private:
		friend class LightSystem;
		LightHandle(Details::LightInfoContainer& _infos, Details::LightKey _key) 
			: m_reference(_infos), m_key(_key) {}

		Details::LightKey m_key;
		Details::LightInfoContainer& m_reference;
	};
	
	class LightSystem
	{
	public:
		static void Initialize();
		static void Close();

		static LightHandle Register(const LightInfo& _info);
		static void DeRegister(const LightHandle& _hndl);

		static void Draw();
	private:
		static Details::LightKey m_currentKey;

		static VertexArrayBuffer* m_vertices;
		static Details::LightInfoContainer m_lightInfos;
	};
}