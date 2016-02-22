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
	
	gradientVecs = new vec2*[perlinSquaresX];
	for (int x = 0; x < perlinSquaresX; x++)
	{
		gradientVecs[x] = new vec2[perlinSquaresY];
		for (int y = 0; y < perlinSquaresY; y++)
		{
			float angle = (rand() / (float)RAND_MAX) * 2 * 3.14159265359f;
			gradientVecs[x][y] = glm::vec2(cosf(angle), sinf(angle)); //size is normalized
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
	
	// get indeces for gradient vecs
	int topLeftIdxX = x / perlinSquareSize;
	int topLeftIdxY = y / perlinSquareSize;

	//get vec from (0.0,0.0) top-left to (1.0,1.0) bottom-right based on pos in Perlin square
	vec2 localPos = vec2((x % perlinSquareSize) / (float)perlinSquareSize, (y % perlinSquareSize) / (float)perlinSquareSize);

	//lerp top 2 gradient vectors in the perlin square
	vec2 xlerp1 = Lerp(gradientVecs[topLeftIdxX][topLeftIdxY], gradientVecs[topLeftIdxX + 1][topLeftIdxY], localPos.x);
	//lerp bottom 2
	vec2 xlerp2 = Lerp(gradientVecs[topLeftIdxX][topLeftIdxY+1], gradientVecs[topLeftIdxX + 1][topLeftIdxY+1], localPos.x);

	//now lerp results along y axis
	vec2 ylerp = Lerp(xlerp1, xlerp2, localPos.y); //our final gradient vector

	//rotate by 90 deg to get curl
	vec2 result = vec2(ylerp.y, -ylerp.x);

	return result;
}

glm::vec2 CurlNoise::Lerp(glm::vec2 a, glm::vec2 b, float t)
{
	return (1 - t)*a + t*b;
}
