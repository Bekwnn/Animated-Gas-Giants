#include "RandomColorGrad.h"
#include <iostream>
#include <random>

RandomColorGrad::RandomColorGrad()
{
	color = glm::vec3(0.5, 0.5, 0.5);
}

RandomColorGrad::RandomColorGrad(int r, int g, int b)
{
	color = glm::vec3(r / 255.f, g / 255.f, b / 255.f);
}

glm::vec3 RandomColorGrad::GetColorNear(float dist)
{
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_real_distribution<float> rdiff(-dist, dist);
	return glm::vec3(fmaxf(0.0f, fminf(color.r + rdiff(e2), 1.0f)), fmaxf(0.0f, fminf(color.g + rdiff(e2), 1.0f)), fmaxf(0.0f, fminf(color.b + rdiff(e2), 1.0f)));
}
