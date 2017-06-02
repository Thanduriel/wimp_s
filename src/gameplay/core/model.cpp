#include "model.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "control/camera.hpp"

namespace Game {
	
	void Model::Draw()
	{
		Graphic::UniformBuffer& objectConstants = Graphic::Resources::GetUBO(Graphic::UniformBuffers::SIMPLE_OBJECT);
		ei::Mat4x4 modelViewProjection = Control::g_camera.GetViewProjection() * m_transformation;
		// openGl expects a different matrix format
		objectConstants["c_WorldViewProjection"] = transpose(modelViewProjection);

		Mesh::Draw();
	}
}