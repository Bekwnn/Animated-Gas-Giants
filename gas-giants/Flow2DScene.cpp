#include "Flow2DScene.h"
#include "ShaderLoader.h"

#include <iostream> //delete later

Flow2DScene::Flow2DScene() : Scene()
{
	vertFragShader = ShaderLoader::CompileVertFrag("GasGiant.vert", "GasGiant.frag");
	advectShader = ShaderLoader::CompileCompute("Advect.comp");

	CurlNoise curlNoise = CurlNoise(32, 32);
	for (int x = 0; x < NOISE_SIZE; x++)
	{
		for (int y = 0; y < NOISE_SIZE; y++)
		{
			glm::vec2 vec = curlNoise.GenerateCurlNoise(x, y, 16);
			weatherTex[x][y][0] = (vec.x + 1.f)/2.f;
			weatherTex[x][y][1] = (vec.y + 1.f)/2.f;
			weatherTex[x][y][2] = 0.f;
		}
	}

	for (int x = 0; x < FLUID_SIZE; x++)
	{
		for (int y = 0; y < FLUID_SIZE; y++)
		{
			fluidDir[x][y][0] = 0.f;
			fluidDir[x][y][1] = 0.f;
			fluidDir[x][y][2] = curlNoise.PerlinNoise(x, y, 16);
		}
	}

	// An array of 3 vectors which represents 3 vertices
	std::vector<GLfloat> verts {
		-1.f, -1.f,  0.f,
		 1.f, -1.f,  0.f,
		-1.f,  1.f,  0.f, //triangle 1
		 1.f, -1.f,  0.f,
		 1.f,  1.f,  0.f,
		-1.f,  1.f,  0.f  //triangle 2
	};
	vertices = verts;

	std::vector<GLfloat> uvs {
		0.f, 0.f,
		1.f, 0.f,
		0.f, 1.f, //triangle 1
		1.f, 0.f,
		1.f, 1.f,
		0.f, 1.f  //triangle 2
	};
	uvcoords = uvs;

	std::vector<GLfloat> checkers {
		1.f, 0.f, 0.f,   0.f, 1.f, 0.f,
		0.f, 0.f, 1.f,   1.f, 1.f, 0.f
	};
	testTex = checkers;

	glGenVertexArrays(1, &VertexArrayID);
	glGenBuffers(1, &BufferID);
	glGenBuffers(1, &UVcoordID);
	glGenTextures(1, &TextureID1);
	glGenTextures(1, &TextureID2);
}

void Flow2DScene::Update()
{
}

void Flow2DScene::RenderScene()
{
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(VertexArrayID);

	glBindBuffer(GL_ARRAY_BUFFER, BufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, UVcoordID);
	glBufferData(GL_ARRAY_BUFFER, uvcoords.size()*sizeof(GLfloat), &uvcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glUseProgram(vertFragShader);

	/*
	// Perform the main fluid simulation steps: advect, diffuse, add forces
	uTemp = advect(u);
	swap(u, uTemp);
	uTemp = diffuse(u);
	swap(u, uTemp);
	uTemp = addForces(u);
	swap(u, uTemp);
	// Now stabilize the result
	p = computePressure(u);
	uTemp = subtractPressureGradient(u, p);
	swap(u, uTemp);
	*/

	glUniform1i(glGetUniformLocation(vertFragShader, "tex1"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID1);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, NOISE_SIZE, NOISE_SIZE);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, NOISE_SIZE, NOISE_SIZE, GL_RGB, GL_FLOAT, weatherTex);

	glUniform1i(glGetUniformLocation(vertFragShader, "tex2"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureID2);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, FLUID_SIZE, FLUID_SIZE);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FLUID_SIZE, FLUID_SIZE, GL_RGB, GL_FLOAT, fluidDir);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
