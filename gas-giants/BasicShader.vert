#version 430 core

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 vertUV;

out vec2 fragUV;

void main() {
    gl_Position = vec4(vert, 1);
	fragUV = vertUV;
}