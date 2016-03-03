

main()
{
	//sample around target location using offset
	vec3 xleft  = texture2D(x, coords - vec2(1,0));
	vec3 xright = texture2D(x, coords + vec2(1,0));
	vec3 xbelow = texture2D(x, coords - vec2(0,1));
	vec3 xabove = texture2D(x, coords + vec2(0,1));

	//sample from center
	vec3 bcenter = texture2D(b, coords);

	//evaluate jacobi iteration
	outColor = (left + right + below + above + alpha * center) * rBeta;
}