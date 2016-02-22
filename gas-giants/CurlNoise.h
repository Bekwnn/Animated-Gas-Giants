#pragma once
#include <glm.hpp>

class CurlNoise {
public:
	CurlNoise();
	CurlNoise(int perlinSquaresX, int perlinSquaresY);
	~CurlNoise();

	glm::vec2 GenerateCurlNoise(int x, int y, int perlinSquareSize = 16);

	glm::vec2** gradientVecs;

private:
	static glm::vec2 Lerp(glm::vec2 a, glm::vec2 b, float t);

};