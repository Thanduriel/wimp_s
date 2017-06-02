#version 330


layout(location=0) in vec3 in_Position;
layout(location=1) in float in_Radius;

out vec3 vs_out_Position;
out float vs_out_Radius;

void main()
{
	// Pass through everything
	vs_out_Position = in_Position;
	vs_out_Radius = in_Radius;
}
