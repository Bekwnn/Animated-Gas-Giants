uniform sampler2D velocity;
uniform sampler2D advected;
uniform sampler2D curlNoise;

uniform float deltat;
uniform float dissipation;

out vec3 color;

void main()
{
	//follow velocity field back a time step
	vec2 oldpos = coords - deltat * texture2D(velocity, uvcoords);

	//bilerp and write to output
	vec3 color = bilerp(advected, oldpos);
}

vec3 bilerpAround(sampler2D tex, vec2 centerPos)
{

}