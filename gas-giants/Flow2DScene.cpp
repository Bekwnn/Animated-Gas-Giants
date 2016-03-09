#include "Flow2DScene.h"
#include "ShaderLoader.h"

#include <iostream> //delete later

Flow2DScene::Flow2DScene() : Scene()
{
	//COMPILE SCENE SHADERS
	vertFragShader = ShaderLoader::CompileVertFrag("GasGiant.vert", "GasGiant.frag");
	advectShader = ShaderLoader::CompileCompute("Advect.comp");
	jacobiShader = ShaderLoader::CompileCompute("JacobiDiffuse.comp");
	divergenceShader = ShaderLoader::CompileCompute("Divergence.comp");
	gradientShader = ShaderLoader::CompileCompute("Gradient.comp");

	//WEATHER VELOCITY FIELD
	// generates velocity field based on curl of perlin noise
	CurlNoise curlNoise = CurlNoise(32, 32);
	for (int x = 0; x < NOISE_SIZE; x++)
	{
		for (int y = 0; y < NOISE_SIZE; y++)
		{
			glm::vec2 vec = curlNoise.GenerateCurlNoise(x, y, 16);
			weatherVelocityField[x][y][0] = (vec.x + 1.f)/2.f;
			weatherVelocityField[x][y][1] = (vec.y + 1.f)/2.f;
			weatherVelocityField[x][y][2] = 0.f;
		}
	}

	//FLUID VELOCITY FIELD
	//initialize to zero or to weather field
	for (int x = 0; x < FLUID_SIZE; x++)
	{
		for (int y = 0; y < FLUID_SIZE; y++)
		{
			fluidVelocityField[x][y][0] = 0.f;
			fluidVelocityField[x][y][1] = 0.f;
			fluidVelocityField[x][y][2] = 0.f;
		}
	}

	//PRESSURE FIELD
	// holds fluid pressure
	for (int x = 0; x < PRESSURE_SIZE; x++)
	{
		for (int y = 0; y < PRESSURE_SIZE; y++)
		{
			pressureField[x][y][0] = 0.f;
			pressureField[x][y][1] = 0.f;
			pressureField[x][y][2] = 0.f;
		}
	}

	//DYE FIELD
	// holds the actual colors of the simulation,
	// initialize to whatever pattern the planet should have
	for (int x = 0; x < DYE_SIZE; x++)
	{
		for (int y = 0; y < DYE_SIZE; y++)
		{
			if (y > DYE_SIZE/4 && y < (DYE_SIZE/4)*3 && x > DYE_SIZE/4 && x < (DYE_SIZE/4)*3)
				pressureField[x][y][0] = 1.f;
			else
				pressureField[x][y][0] = 0.f;
			pressureField[x][y][1] = 0.f;
			pressureField[x][y][2] = 0.f;
		}
	}

	//INITIALIZE MESHES AND UVS
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

	//GENERATE OPENGL OBJECTS
	glGenVertexArrays(1, &VertexArrayID);
	glGenBuffers(1, &BufferID);
	glGenBuffers(1, &UVcoordID);

	glGenTextures(1, &weatherTex);
	glGenTextures(1, &fluidVelocityTex);
	glGenTextures(1, &pressureTex);
	glGenTextures(1, &dyeTex);
}

void Flow2DScene::Update()
{
}

void Flow2DScene::RenderScene()
{
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Flow2DScene::ComputeFluidImages()
{
	//PERFORM MAIN FLUID SIMULATION COMPUTATION STEPS
	/*
	glUseProgram(advectShader);
	glBindImageTexture(2, weatherTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glUniform1f(glGetUniformLocation(advectShader, "deltaTime"), time.deltaTime);
	glUniform1f(glGetUniformLocation(advectShader, "dissipation"), 0.2f);

	glDispatchCompute(1, 128, 1);
	*/
	//u = advect(u);
	//u = diffuse(u);
	// Now stabilize the result
	//p = computePressure(u);
	//u = subtractPressureGradient(u, p);
}

void Flow2DScene::RenderMeshes()
{
	//BIND VERTEX BUFFERS
	glBindVertexArray(VertexArrayID);

	glBindBuffer(GL_ARRAY_BUFFER, BufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, UVcoordID);
	glBufferData(GL_ARRAY_BUFFER, uvcoords.size()*sizeof(GLfloat), &uvcoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//RENDER RESULT WITH VERTEX AND FRAGMENT SHADER
	glUseProgram(vertFragShader);

	//bind and store texture 1
	glUniform1i(glGetUniformLocation(vertFragShader, "tex1"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, weatherTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, NOISE_SIZE, NOISE_SIZE);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, NOISE_SIZE, NOISE_SIZE, GL_RGB, GL_FLOAT, weatherVelocityField);

	//bind and store texture 2
	glUniform1i(glGetUniformLocation(vertFragShader, "tex2"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fluidVelocityTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, FLUID_SIZE, FLUID_SIZE);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FLUID_SIZE, FLUID_SIZE, GL_RGB, GL_FLOAT, fluidVelocityField);

	//draw full screen quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
}