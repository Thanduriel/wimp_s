#version 330
#extension GL_ARB_explicit_uniform_location : enable

in vec3 vs_out_Position[1];
in float vs_out_Radius[1];
in vec3 vs_out_Color[1];

layout(std140) uniform Object
{
	mat4 c_mViewProjection;
};

#include "globalubo.glsl"
#include "cameraubo.glsl"

out vec2 gs_texCoord;
out vec4 gs_center;
out vec3 gs_eyeDirection;
out vec3 gs_color;
out float gs_radius;
out float gs_fallOff;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main(void)
{
	vec3 color = vs_out_Color[0].xyz;
	float size = vs_out_Radius[0];
	// intensity fall of can be calculated once per light
	const float threshold = 0.25; // this const needs to be updated in light.ps aswell.
	float fallOff = 1/(size * size * threshold);

	vec4 center = vec4(vs_out_Position[0].xyz, 1) * c_mView;//vec4(position.xyz * c_vInverseProjection.xyz + vec3(0,0,c_vInverseProjection.w), 1);//position / position.w;
	vec4 position = center;
//	position.z += size;
	vec3 cubeFace = normalize(position.xyz) * size;
	if(cubeFace.z < 0) cubeFace *= -1;
	
	vec3 farP = position.xyz + cubeFace;
	vec3 nearP = position.xyz - cubeFace;
	float altPosZ;
	
	// manual cliping since no point of the sphere can be seen
	if(farP.z <= 0)
		return;//altPosZ = farP.z;
	else if(nearP.z >= 0) // whole volume is in front -> use the closest plane
		altPosZ = nearP.z;
	else // camera is inside the volume -> use camera plane
	{
	//	float l = - position.z / cubeFace.z;
		altPosZ = 0.0;//vec3(position.xy  + cubeFace.xy * l, 0);
	//	color = vec3(0,0,1);
	}
	
	// the clip plane is always the one furthest in front of the camera
	position.xyz = farP;
	position = vec4(position.xyz * c_vProjection.xyz + vec3(0,0,c_vProjection.w), position.z);

	// only z component is required for the correct size
	altPosZ = altPosZ * c_vProjection.z + c_vProjection.w;
	size *= position.z / altPosZ;
	
	gl_Position = position + vec4(-size, -size* c_fAspect, 0, 0);// * vec4(c_vProjection.xy,0,0);
//	gl_Position.xy = gl_Position.xy * c_vProjection.xy; gl_Position.y /= c_fAspect;
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = size;
	gs_fallOff = fallOff;
	gs_color = color;
	EmitVertex();

	gl_Position = position + vec4( size, -size* c_fAspect , 0, 0);// * vec4(c_vProjection.xy,0,0);
//	gl_Position.xy = gl_Position.xy * c_vProjection.xy; gl_Position.y /= c_fAspect;
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = size;
	gs_fallOff = fallOff;
	gs_color = color;
	EmitVertex();

	gl_Position = position + vec4(-size,  size* c_fAspect, 0, 0);// * vec4(c_vProjection.xy,0,0);
//	gl_Position.xy = gl_Position.xy * c_vProjection.xy; gl_Position.y /= c_fAspect;
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = size;
	gs_fallOff = fallOff;
	gs_color = color;
	EmitVertex();

	gl_Position = position + vec4( size,  size* c_fAspect, 0, 0);// * vec4(c_vProjection.xy,0,0);
//	gl_Position.xy = gl_Position.xy * c_vProjection.xy; gl_Position.y /= c_fAspect;
//	gl_Position.xy = vec4(vec3(gl_Position.xy, gl_Position.z + size) * c_vProjection.xyz + vec3(0,0,c_vProjection.w), 1).xy;
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = size;
	gs_fallOff = fallOff;
	gs_color = color;
	EmitVertex();
	EndPrimitive();
}