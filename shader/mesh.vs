#version 330

layout(location = 0) in vec3 vs_in_position;
layout(location = 1) in vec3 vs_in_normal;
layout(location = 2) in vec2 vs_in_texCoords;

layout(std140) uniform Object
{
	mat4 c_mWorldViewProjection;
};

out vec4 vs_out_normal;
out vec2 vs_out_texCoords;

void main()
{
	gl_Position = vec4(vs_in_position, 1.0) * c_mWorldViewProjection;
	vs_out_normal =/* c_ModelViewProjection * */ vec4(vs_in_normal, 0.0);
	vs_out_texCoords = vs_in_texCoords;
}