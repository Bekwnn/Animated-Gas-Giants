#include "RandomColorGrad.h"
#include <iostream>

RandomColorGrad::RandomColorGrad()
{
	color = glm::vec3(0.5, 0.5, 0.5);
}

RandomColorGrad::RandomColorGrad(float r, float g, float b)
{
	color = glm::vec3(fmaxf(0.0f, fminf(1.0f, r)), fmaxf(0.0f, fminf(1.0f, g)), fmaxf(0.0f, fminf(1.0f, b)));
}

glm::vec3 RandomColorGrad::GetColorNear(float dist)
{
	float rdiff = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / dist)) - 0.5f * dist;
	float gdiff = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / dist)) - 0.5f * dist;
	float bdiff = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / dist)) - 0.5f * dist;
	return glm::vec3(fmaxf(0.0f, fminf(color.r + rdiff, 1.0f)), fmaxf(0.0f, fminf(color.g + gdiff, 1.0f)), fmaxf(0.0f, fminf(color.b + bdiff, 1.0f)));
}
