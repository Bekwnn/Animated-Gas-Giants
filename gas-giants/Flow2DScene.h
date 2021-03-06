#pragma once
#include "Scene.h"
#include "CurlNoise.h"

#include <vector>

#define NOISE_SIZE 512
#define FLUID_SIZE 512
#define PRESSURE_SIZE 512
#define DYE_SIZE 512

class Flow2DScene : public Scene
{
public:
	Flow2DScene();

	void Update() override;
	void RenderScene() override;

	void RenderMeshes();
	void ComputeFluidImages();

	GLuint vertFragShader;
	GLuint advectShader;
	GLuint jacobiShader;
	GLuint divergenceShader;
	GLuint gradientShader;
	GLuint forcesShader;
	GLuint testCompShader; //for testing

	//used to build initial texture values
	float weatherVelocityField[NOISE_SIZE][NOISE_SIZE][3];
	float fluidVelocityField[FLUID_SIZE][FLUID_SIZE][3];
	float pressureField[PRESSURE_SIZE][PRESSURE_SIZE][3];
	float dyeField[DYE_SIZE][DYE_SIZE][3];

	GLuint VertexArrayID;
	GLuint BufferID;
	GLuint UVcoordID;

	GLuint weatherTex;
	GLuint fluidVelocityTex;
	GLuint pressureTex;
	GLuint dyeTex;
	GLuint divergenceTex;
	GLuint tempTransferTex;

	std::vector<GLfloat> vertices;
	std::vector<GLfloat> uvcoords;
	std::vector<GLfloat> testTex;
};