#version 330
#extension GL_ARB_explicit_uniform_location : enable

in vec3 vs_out_Position[1];
in float vs_out_Radius[1];
in vec4 vs_out_color[1];

layout(std140) uniform Object
{
	mat4 c_mViewProjection;
};

#include "globalubo.glsl"
#include "cameraubo.glsl"

out vec2 gs_texCoord;
out vec4 gs_center;
out vec3 gs_eyeDirection;
out float gs_radius;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main(void)
{
	vec4 position = vec4(vs_out_Position[0].xyz, 1) * c_mViewProjection;
	vec4 center = vec4(position.xyz * c_vInverseProjection.xyz + vec3(0,0,c_vInverseProjection.w), 1);//position / position.w;
	
	float size = vs_out_Radius[0];

	gl_Position = position + vec4(-size, -size * c_fAspect, 0, 0);
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = size;
	EmitVertex();

	gl_Position = position + vec4( size, -size * c_fAspect, 0, 0);
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = size;
	EmitVertex();

	gl_Position = position + vec4(-size,  size * c_fAspect, 0, 0);
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = size;
	EmitVertex();

	gl_Position = position + vec4( size,  size * c_fAspect, 0, 0);
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = size;
	EmitVertex();
	EndPrimitive();
}