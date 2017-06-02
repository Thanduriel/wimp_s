#version 330
#extension GL_ARB_explicit_uniform_location : enable

in vec3 vs_out_Position[1];
in float vs_out_Radius[1];

layout(std140) uniform Object
{
	mat4 c_mWorldViewProjection;
};

#include "../globalubo.glsl"

out vec2 gs_texCoord;
out vec2 gs_center;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main(void)
{
	vec4 position = vec4(vs_out_Position[0].xyz, 1) * c_mWorldViewProjection;
	gs_center = position.xy;
	
	float size = vs_out_Radius[0];
	gs_texCoord = vec2(-1.0, -1.0);
	gl_Position = position + vec4(-size, -size * c_fAspect, 0, 0);
	EmitVertex();
	gs_texCoord = vec2(1.0, -1.0);
	gl_Position = position + vec4( size, -size * c_fAspect, 0, 0);
	EmitVertex();
	gs_texCoord = vec2(-1.0, 1.0);
	gl_Position = position + vec4(-size,  size * c_fAspect, 0, 0);
	EmitVertex();
	gs_texCoord = vec2(1.0, 1.0);
	gl_Position = position + vec4( size,  size * c_fAspect, 0, 0);
	EmitVertex();
	EndPrimitive();
}