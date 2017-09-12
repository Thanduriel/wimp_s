#version 330
#extension GL_ARB_explicit_uniform_location : enable

in vec3 vs_out_Position[1];
in float vs_out_Radius[1];
in vec3 vs_out_Color[1];
in float vs_out_Intensity[1];

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
out float gs_intensity;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

void main(void)
{
	vec3 color = vs_out_Color[0].xyz;
	float size = vs_out_Radius[0];
	float radSq = vs_out_Radius[0] * vs_out_Radius[0];
	float intensity = vs_out_Intensity[0];

	vec4 center = vec4(vs_out_Position[0].xyz, 1) * c_mView;//vec4(position.xyz * c_vInverseProjection.xyz + vec3(0,0,c_vInverseProjection.w), 1);//position / position.w;
	vec4 position = center;
//	position.z += size;
	vec3 cubeFace = normalize(position.xyz) * size;
	if(cubeFace.z < 0) cubeFace *= -1;
	
	vec3 farP = position.xyz + cubeFace;
	vec3 nearP = position.xyz - cubeFace;
	float altPosZ;
	
	// manual cliping since no point of the sphere can be seen
	if(position.z + size <= 0)
		return;//altPosZ = farP.z;
	else if(nearP.z >= 0) // whole volume is in front -> use the closest plane
	{
		altPosZ = nearP.z;
		// the clip plane is always the one furthest in front of the camera
		position.xyz = farP;
	}
	else // camera is inside the volume -> use camera plane
	{
	//	float l = - position.z / cubeFace.z;
		altPosZ = 0.0;//vec3(position.xy  + cubeFace.xy * l, 0);
		// 
		position.z += size + 0.1;
	//	color = vec3(0,0,1);
	}
	
	position = vec4(position.xyz * c_vProjection.xyz + vec3(0,0,c_vProjection.w), position.z);
	// only z component is required for the correct size
	altPosZ = altPosZ * c_vProjection.z + c_vProjection.w;
	if(abs(position.z) <= 0.1) {position.z = 0.1; /*altPosZ = 0.01;*/}
	size *= position.z / altPosZ;
	
	gl_Position = position + vec4(-size, -size* c_fAspect, 0, 0);// * vec4(c_vProjection.xy,0,0);
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = radSq;
	gs_color = color;
	gs_intensity = intensity;
	EmitVertex();

	gl_Position = position + vec4( size, -size* c_fAspect , 0, 0);// * vec4(c_vProjection.xy,0,0);
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = radSq;
	gs_color = color;
	gs_intensity = intensity;
	EmitVertex();

	gl_Position = position + vec4(-size,  size* c_fAspect, 0, 0);// * vec4(c_vProjection.xy,0,0);
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = radSq;
	gs_color = color;
	gs_intensity = intensity;
	EmitVertex();

	gl_Position = position + vec4( size,  size* c_fAspect, 0, 0);// * vec4(c_vProjection.xy,0,0);
	gs_texCoord = gl_Position.xy / gl_Position.w;
	gs_eyeDirection = vec3(gs_texCoord * c_vInverseProjection.xy , 1.0);
	gs_texCoord = gs_texCoord * 0.5 + 0.5;
	gs_center = center;
	gs_radius = radSq;
	gs_color = color;
	gs_intensity = intensity;
	EmitVertex();
	EndPrimitive();
}