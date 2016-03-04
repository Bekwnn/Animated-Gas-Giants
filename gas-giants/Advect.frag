// ADVECTION STEP:
// Transports color from the source texture along the velocity field.
// Does so by travelling back along the velocity field by a time step
// then grabbing the color from that old position and moving it to the
// new position.

uniform sampler2D velocityTex;// velocity field
uniform sampler2D sourceTex; // source texture of final colors

uniform float deltaTime;
uniform float dissipation;

//TODO: assuming gridscale = 1 for now, for performance use smaller grid
// and bilerp along it

//advected color quantity (final value)
out vec3 color;

void main()
{
	//follow velocity field back a time step
	vec2 oldpos = coords - deltat * texture2D(velocity, uvcoords);

	//dissipate and return
	vec3 color = dissipation * texture2D(sourceTex, oldpos);
}

vec3 bilerp(sampler2D tex, vec2 pos)
{
	//TODO:
	return vec3();
}
