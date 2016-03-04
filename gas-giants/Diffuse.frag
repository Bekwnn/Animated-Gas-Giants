

main()
{
	//sample around target location using offset
	vec3 xright = textureOffset(tex, coord, vec2( 1, 0));
	vec3 xleft  = textureOffset(tex, coord, vec2(-1, 0));
	vec3 xabove = textureOffset(tex, coord, vec2( 0, 1));
	vec3 xbelow = textureOffset(tex, coord, vec2( 0,-1));

	//sample from center
	vec3 bcenter = texture2D(b, coords);

	//evaluate jacobi iteration
	outColor = (xleft + xright + xbelow + xabove + alpha * bcenter) * rBeta;
}
