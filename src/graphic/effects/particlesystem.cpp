#include "particlesystem.hpp"
#include <algorithm>
#include "graphic/core/device.hpp"
#include "graphic/resources.hpp"
#include "control/camera.hpp"
#include "graphic/core/uniformbuffer.hpp"

using namespace ei;

namespace Graphic {
namespace ParticleSystems{


// ************************************************************************* //
// SystemAction class
ParticleSystems::SystemActions::SystemActions(RenderType _type) :
	m_particleVertices(VertexArrayBuffer::PrimitiveType::POINT),
	m_renderer(_type),
	m_texture(nullptr)
{
	m_particleVertices.SetNumInstances(1);
}

SystemActions::SystemActions(const Texture& _texture):
	m_particleVertices(VertexArrayBuffer::PrimitiveType::POINT),
	m_renderer(RenderType::TEXQUAD),
	m_texture(&_texture)
{}

void ParticleSystems::SystemActions::Draw( size_t _numVertices )
{
	// Set renderer and do draw call. The set has no effect if the renderer is
	// currently active.
	switch(m_renderer)
	{
	case RenderType::BLOB:
		Device::SetEffect(	Resources::GetEffect(Effects::PARTICLE_BLOB) );
		break;
	case RenderType::RAY:
		Device::SetEffect(Resources::GetEffect(Effects::PARTICLE_RAY));
		break;
	case RenderType::TEXQUAD:
		Device::SetEffect(Resources::GetEffect(Effects::PARTICLE_TEXQAUD));
		Device::SetTexture(*m_texture, 0);
		break;
	default:
		Assert(false, "This effect is not implemented.");
	}
	m_particleVertices.Bind();
	Graphic::Device::DrawVertices( m_particleVertices, 0, (unsigned)_numVertices);
}


// ************************************************************************* //
// Position Components
PositionComponents::PositionComponents()
{
	m_positions = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({{VertexAttribute::VEC3, 0}}), false);
}

void PositionComponents::Add(const ei::Vec3& _pos)
{
	m_positions->Add(_pos);
	m_positions->Touch();
}

void PositionComponents::Remove(size_t _idx)
{
	m_positions->Remove<ei::Vec3>((int)_idx);
	m_positions->Touch();
}

void PositionComponents::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_positions);
}

// ************************************************************************* //
// Color components
ColorComponents::ColorComponents()
{
	m_colors = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({{VertexAttribute::COLOR, 3}}), false);
}

void ColorComponents::Add(uint32 _color)
{
	m_colors->Add(_color);
	m_colors->Touch();
}

void ColorComponents::Remove(size_t _idx)
{
	m_colors->Remove<uint32>((int)_idx);
	m_colors->Touch();
}

void ColorComponents::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_colors);
}

// System wide color
PSColorComponent::PSColorComponent()
{
	m_systemColor = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({{VertexAttribute::COLOR, 3}}), false);
	// Default to black
	m_systemColor->Add<uint32>(0);
	m_systemColor->Touch();
}

void PSColorComponent::SetColor(uint32 _color)
{
	*(uint32*)m_systemColor->GetDirectAccess() = _color;
	m_systemColor->Touch();
}

void PSColorComponent::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_systemColor);
}


// ************************************************************************* //
// Size components
SizeComponents::SizeComponents()
{
	m_sizes = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({{VertexAttribute::FLOAT, 1}}), false);
}

void SizeComponents::Add(float _size)
{
	m_sizes->Add(_size);
	m_sizes->Touch();
}

void SizeComponents::Remove(size_t _idx)
{
	m_sizes->Remove<float>((int)_idx);
	m_sizes->Touch();
}

void SizeComponents::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_sizes);
}

PSSizeComponent::PSSizeComponent()
{
	m_systemSize = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({{VertexAttribute::FLOAT, 1}}), true);
	// Default to 0.5 (well visible)
	m_systemSize->Add<float>(0.5f);
	m_systemSize->Touch();
}

void PSSizeComponent::SetParticleSize(float _size)
{
	*(float*)m_systemSize->GetDirectAccess() = _size;
	m_systemSize->Touch();
}

void PSSizeComponent::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_systemSize);
}

// ************************************************************************* //
// direction components
DirectionComponents::DirectionComponents()
{
	m_directions = std::make_shared<DataBuffer>(std::initializer_list<VertexAttribute>({ { VertexAttribute::VEC3, 4 } }), false);
}

void DirectionComponents::Add(const ei::Vec3& _pos)
{
	m_directions->Add(_pos);
	m_directions->Touch();
}

void DirectionComponents::Remove(size_t _idx)
{
	m_directions->Remove<ei::Vec3>((int)_idx);
	m_directions->Touch();
}

void DirectionComponents::AttachTo(VertexArrayBuffer& _vertexArray)
{
	_vertexArray.AttachBuffer(m_directions);
}

// ************************************************************************* //
// system manager
std::vector< Manager::SystemPair > Manager::m_particleSystems;

void Manager::Close()
{
	m_particleSystems.clear();
}

void Manager::Draw(const Control::Camera& _camera)
{
	// Upload current transformation matrix
	Resources::GetUBO(UniformBuffers::SIMPLE_OBJECT)["c_WorldViewProjection"] = _camera.GetViewProjection();

	for (auto& system : m_particleSystems)
	{
		system.second->Draw(system.second->GetNumParticles());
	}
}

void Manager::Process(float _deltaTime)
{
	for (auto& system : m_particleSystems)
		system.second->Simulate(_deltaTime);
}

void Manager::CleanUp()
{
	for (auto& system : m_particleSystems)
		system.second->CleanUp();
}

size_t Manager::GetNumParticlesTotal()
{
	size_t count = 0;

	for (auto& system : m_particleSystems)
		count += system.second->GetNumParticles();

	return count;
}


} // namespace ParticleSystem
} // namespace Graphic