#version 430 core

uniform sampler2D tex;

in vec2 fragUV;

out vec3 color;

void main() {
    color = texture2D(tex, fragUV).rgb;
	//color = vec3(fragUV.x, fragUV.y, 0); //works
}