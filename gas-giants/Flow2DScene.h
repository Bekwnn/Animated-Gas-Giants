#pragma once
#include "Scene.h"
#include "CurlNoise.h"

#include <vector>

#define NOISE_SIZE 128
#define FLUID_SIZE 128

class Flow2DScene : public Scene
{
public:
	Flow2DScene();

	void Update() override;
	void RenderScene() override;

	GLuint vertFragShader;
	GLuint advectShader;
	GLuint jacobiShader;
	GLuint divergenceShader;

	float weatherTex[NOISE_SIZE][NOISE_SIZE][3];
	float fluidDir[FLUID_SIZE][FLUID_SIZE][3];

	GLuint VertexArrayID;
	GLuint BufferID;
	GLuint UVcoordID;
	GLuint TextureID1;
	GLuint TextureID2;

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> uvcoords;
	std::vector<GLfloat> testTex;
};