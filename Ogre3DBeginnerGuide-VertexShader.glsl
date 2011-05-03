#version 330

in vec4 position;
uniform mat4 worldViewMatrix;

void main()
{
	gl_Position = worldViewMatrix * position;
}