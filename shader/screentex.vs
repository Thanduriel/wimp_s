#version 330


layout(location=11) in vec2 in_Position;
layout(location=12) in vec2 in_Position2;
layout(location=13) in vec2 in_TexCoord;
layout(location=14) in vec2 in_Size;
layout(location=15) in vec4 in_Color;

out vec2 vs_out_Position;
out vec2 vs_out_Position2;
out vec2 vs_out_TexCoord;
out vec2 vs_out_Size;
out vec4 vs_out_Color;

void main()
{
	// Pass through everything
	vs_out_Position = in_Position;
	vs_out_Position2 = in_Position2;
	vs_out_TexCoord = in_TexCoord;
	vs_out_Size = in_Size;
	vs_out_Color = in_Color;
}
