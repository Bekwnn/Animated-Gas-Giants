uniform sampler2D velocity;
uniform sampler2D advected;
uniform sampler2D curlNoise;

uniform float deltat;

out vec3 color;

void main()
{
	vec2 pos = coords - deltat * texture2D(velocity, uvcoords);
}