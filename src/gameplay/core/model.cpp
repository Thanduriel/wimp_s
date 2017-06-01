#include "model.hpp"
#include "graphic/resources.hpp"
#include "graphic/core/uniformbuffer.hpp"
#include "control/camera.hpp"

namespace Game {
	
	void Model::Draw()
	{
		Graphic::UniformBuffer& objectConstants = Graphic::Resources::GetUBO(Graphic::UniformBuffers::OBJECT_MESH);
		ei::Mat4x4 modelViewProjection = Control::g_camera.GetViewProjection() * m_transformation;
		// openGl expects a different matrix format
		objectConstants["c_ModelViewProjection"] = transpose(modelViewProjection);

		Mesh::Draw();
	}
}