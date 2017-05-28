#version 330 core

layout(location = 0) in vec3 position;

out vec4 vs_position;

void main()
{
	vs_position = vec4(position, 1);
}
