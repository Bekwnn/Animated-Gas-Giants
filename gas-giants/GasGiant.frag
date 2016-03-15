#version 430 core

uniform sampler2D tex1;
uniform sampler2D tex2;

in vec2 fragUV;

out vec3 color;

void main() {
    color = texture(tex1, fragUV).rgb + texture(tex2, fragUV).rgb;
	//color = texture2D(tex2, fragUV).rgb;
	//color = texture2D(tex1, fragUV).rgb;
}