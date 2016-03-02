#pragma once
#include <glm.hpp>

class CurlNoise {
public:
	//TODO: add numHalfsizeLayers to constructor 
	//which would create additional gradient vectors at 2x, 4x, 8x etc resolution
	CurlNoise();
	CurlNoise(int perlinSquaresX, int perlinSquaresY);
	~CurlNoise();

	glm::vec2 GenerateCurlNoise(int x, int y, int perlinSquareSize = 16);
	float PerlinNoise(int x, int y, int perlinSquareSize = 16);

	glm::vec2** gradientVecs;

private:
	static float EaseInterp(float a, float b, float t);

};