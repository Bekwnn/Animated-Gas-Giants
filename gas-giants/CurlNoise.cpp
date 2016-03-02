#include "CurlNoise.h"

#include <ctime>
#include <cmath>

CurlNoise::CurlNoise() :
	CurlNoise(16, 16)
{
	
}

CurlNoise::CurlNoise(int perlinSquaresX, int perlinSquaresY)
{
	using namespace glm;

	srand(time(NULL));
	
	gradientVecs = new vec2*[perlinSquaresX+1];
	for (int x = 0; x < perlinSquaresX+1; x++)
	{
		gradientVecs[x] = new vec2[perlinSquaresY+1];
		for (int y = 0; y < perlinSquaresY+1; y++)
		{
			float angle = (rand() / (float)RAND_MAX) * 2 * 3.14159265359f;
			gradientVecs[x][y] = glm::normalize(glm::vec2(cosf(angle), sinf(angle)))*2.f; //polar coords for even direction distribution
		}
	}
}

CurlNoise::~CurlNoise()
{
	delete gradientVecs;
}

glm::vec2 CurlNoise::GenerateCurlNoise(int x, int y, int perlinSquareSize)
{
	using namespace glm;
	
	//TODO: image borders (ie, if x+1 does not exist, use 2.f*delta of x and x-1 instead)
	float perlinVal = PerlinNoise(x, y, perlinSquareSize);

	float deltax;
	if (x+1 > sizeof(*gradientVecs)/sizeof(**gradientVecs))
		deltax = 2.f * (perlinVal - PerlinNoise(x - 1, y, perlinSquareSize));
	else if (x-1 < 0)
		deltax = 2.f * (PerlinNoise(x + 1, y, perlinSquareSize) - perlinVal);
	else
		deltax = PerlinNoise(x + 1, y, perlinSquareSize) - PerlinNoise(x - 1, y, perlinSquareSize);


	float deltay;
	if (y+1 > sizeof(*gradientVecs)/sizeof(**gradientVecs))
		deltay = 2.f * (perlinVal - PerlinNoise(x, y - 1, perlinSquareSize));
	else if (y-1 < 0)
		deltay = 2.f * (PerlinNoise(x, y + 1, perlinSquareSize) - perlinVal);
	else
		deltay = PerlinNoise(x, y + 1, perlinSquareSize) - PerlinNoise(x, y - 1, perlinSquareSize);

	return vec2(deltay, -deltax)*2.f;
}

float CurlNoise::PerlinNoise(int x, int y, int perlinSquareSize)
{
	using namespace glm;
	// get indeces for gradient vecs
	int topLeftIdxX = x / perlinSquareSize;
	int topLeftIdxY = y / perlinSquareSize;

	//get vec from (0.0,0.0) bot-left to (1.0,1.0) top-right based on pos in Perlin square
	vec2 localPos = vec2((x % perlinSquareSize) / (float)perlinSquareSize, (y % perlinSquareSize) / (float)perlinSquareSize);

	vec2 fromBotLeft = localPos;
	vec2 fromBotRight = localPos - vec2(1.0f, 0.0f);
	vec2 fromTopLeft  = localPos - vec2(0.0f, 1.0f);
	vec2 fromTopRight = localPos - vec2(1.0f, 1.0f);

	float dotBotLeft = glm::dot(gradientVecs[topLeftIdxX][topLeftIdxY], fromBotLeft);
	float dotBotRight = glm::dot(gradientVecs[topLeftIdxX + 1][topLeftIdxY], fromBotRight);
	float dotTopLeft  = glm::dot(gradientVecs[topLeftIdxX][topLeftIdxY + 1], fromTopLeft);
	float dotTopRight = glm::dot(gradientVecs[topLeftIdxX + 1][topLeftIdxY + 1], fromTopRight);

	float xlerp1 = CurlNoise::EaseInterp(dotBotLeft, dotBotRight, localPos.x);
	float xlerp2 = CurlNoise::EaseInterp(dotTopLeft, dotTopRight, localPos.x);

	float ylerp = CurlNoise::EaseInterp(xlerp1, xlerp2, localPos.y);

	return ylerp;
}

float CurlNoise::EaseInterp(float a, float b, float t)
{
	float t3 = t*t*t;
	float t4 = t3*t;
	float t5 = t4*t;
	float t3m = (1 - t)*(1 - t)*(1 - t);
	float t4m = t3m*(1 - t);
	float t5m = t4m*(1 - t);
	return a*(6 * t5m - 15 * t4m + 10 * t3m) + b*(6 * t5 - 15 * t4 + 10 * t3);
}
