#version 330

in vec2 screenCoord;

out vec3 color;

uniform sampler2D leftTexture;

void main()
{
	//if (gl_FragCoord.x < 0.5)
	color = texture( leftTexture, screenCoord.xy).rgb;
}