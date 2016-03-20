#pragma once

#include "glm.hpp"

class RandomColorGrad {
public:
	glm::vec3 color;

	RandomColorGrad();
	RandomColorGrad(float r, float g, float b);

	glm::vec3 GetColorNear(float dist);
};