#version 330

// use same layout as mesh to reuse the vertex buffer
layout(location = 0) in vec3 vs_in_position;
layout(location = 1) in vec3 vs_in_normal;
layout(location = 2) in vec2 vs_in_texCoords;

layout(std140) uniform Object
{
	mat4 c_mWorldViewProjection;
	vec3 c_vPointOfOrigin;
	float c_fLocalTime;
};

out vec3 vs_out_position;
out vec4 vs_out_normal;
out vec2 vs_out_texCoords;

void main()
{
	vec4 position = vec4(vs_in_position, 1.0) * c_mWorldViewProjection;
	vs_out_position = vs_in_position.xyz;
	gl_Position = position;
//	vs_out_normal = vec4(vs_in_normal, 0.0) * c_mNormalWorldView;
//	vs_out_texCoords = vs_in_texCoords;
}