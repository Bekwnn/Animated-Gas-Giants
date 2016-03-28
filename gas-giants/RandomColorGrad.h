#pragma once

#include "glm.hpp"

class RandomColorGrad {
public:
	glm::vec3 color;

	RandomColorGrad();
	RandomColorGrad(int r, int g, int b);

	glm::vec3 GetColorNear(float dist);
};