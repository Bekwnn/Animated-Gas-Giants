#version 430 core

layout(location = 0) in vec4 vPosition;

void main()
{
	gl_Position = vec4(vPosition.x, vPosition.y, 0.0f, 1.0f);
}