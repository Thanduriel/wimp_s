#version 330

#define OUT_VERTS 4

out vec4 gs_Color;
out vec2 gs_TexCoord;

in vec4 vs_position[2];

layout(std140) uniform Object
{
	mat4 c_mModelViewProjection;
	vec4 c_vColor;
	float c_fThickness;
};

layout(lines) in;
layout(triangle_strip, max_vertices = OUT_VERTS) out;

void main()
{
	vec4 l1 = vs_position[0] * c_mModelViewProjection ;
	vec4 l2 = vs_position[1] * c_mModelViewProjection;
	
	// Do a manual clipping.
	float threshold = -5.0; // TODO: use nearplane
	vec4 direction = normalize(l2 - l1);
	// Compute original distance for texture coordinate reprojection.
	float len = length(l2.xyz - l1.xyz) * 0.5;
	// Reproject end points to the near-thresholdplane
	if( l1.z < threshold )
		l1 -= direction * (l1.z-threshold) / direction.z;
	float len2 = length(l2.xyz - l1.xyz);
	float tex1 = 1.0 - len2 / len;
	if( l2.z < threshold )
		l2 -= direction * (l2.z-threshold) / direction.z;
	float tex2 = length(l2.xyz - l1.xyz) / len2 * 2.0 - 1.0;

	// Compute a vector perpendicular vector to create a beam
	vec2 dir = normalize(l2.xy / l2.w - l1.xy / l1.w);
	// Cross product with view direction
	vec4 perpendicular = vec4(-dir.y * c_fThickness, dir.x * c_fThickness, 0, 0);

	gs_Color = c_vColor;
	gl_Position = l1 + perpendicular;
	gs_TexCoord = vec2(tex1, -1.0);
	EmitVertex();
	
	gl_Position = l1 - perpendicular;
	gs_TexCoord = vec2(tex1, 1.0);
	EmitVertex();
	
//	gs_Color = uColor;
	gl_Position = l2 + perpendicular;
	gs_TexCoord = vec2(tex2, -1.0);
	EmitVertex();
	
	gl_Position = l2 - perpendicular;
	gs_TexCoord = vec2(tex2, 1.0);
	EmitVertex();
	EndPrimitive();

/*	float len = length(vs_position[1].xyz - vs_position[0].xyz) * 0.5;
	float len2 = length(vs_position[1].xyz - vs_position[0].xyz);
	float tex1 = 1.0 - len2 / len;
	float tex2 = length(vs_position[1].xyz - vs_position[0].xyz) / len2 * 2.0 - 1.0;
	
	vec4 orth = normalize(vs_position[1] - vs_position[0]);
	orth = vec4(-orth.y, orth.x,0,0);
	
	gs_Color = vs_color[0];
	gl_Position = vs_position[0] + orth;
	gs_TexCoord = vec2(tex1, -1.0);
	EmitVertex();
	
	gl_Position = vs_position[0] - orth;
	gs_TexCoord = vec2(tex1, 1.0);
	EmitVertex();
	
	gl_Position = vs_position[1] - orth;
	gs_TexCoord = vec2(tex2, -1.0);
	EmitVertex();
	
	gl_Position = vs_position[1] + orth;
	gs_TexCoord = vec2(tex2, 1.0);
	EmitVertex();
	EndPrimitive();*/
}