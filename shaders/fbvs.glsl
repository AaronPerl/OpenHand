#version 330

layout(location = 0) in vec4 in_position;

out vec2 screenCoord;

void main()
{
    gl_Position = in_position;
	screenCoord = in_position.xy * 0.5 + 0.5;
}