#include "Flow2DScene.h"
#include "ShaderLoader.h"

Flow2DScene::Flow2DScene() : Scene()
{
	//COMPILE SCENE SHADERS
	vertFragShader = ShaderLoader::CompileVertFrag("GasGiant.vert", "GasGiant.frag");
	advectShader = ShaderLoader::CompileCompute("Advect.comp");
	jacobiShader = ShaderLoader::CompileCompute("JacobiDiffuse.comp");
	divergenceShader = ShaderLoader::CompileCompute("Divergence.comp");
	gradientShader = ShaderLoader::CompileCompute("Gradient.comp");
	testCompShader = ShaderLoader::CompileCompute("BasicShader.comp");

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
			if (y > PRESSURE_SIZE / 4 && y < (PRESSURE_SIZE / 4) * 3 &&
				x > PRESSURE_SIZE / 4 && x < (PRESSURE_SIZE / 4) * 3)
			{
				pressureField[x][y][0] = 1.f;
			}
			else
			{
				pressureField[x][y][0] = 0.f;
			}
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
				dyeField[x][y][0] = 1.f;
			else
				dyeField[x][y][0] = 0.f;
			dyeField[x][y][1] = 0.f;
			dyeField[x][y][2] = 0.f;
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

	printf("weatherTex: %d\n", weatherTex);
	printf("fluidVelocityTex: %d\n", fluidVelocityTex);
	printf("pressureTex: %d\n", pressureTex);
	printf("dyeTex: %d\n", dyeTex);
}

void Flow2DScene::Update()
{
}

void Flow2DScene::RenderScene()
{
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//ComputeFluidImages();

	///*// TEST CODE THAT WORKS:
	//input tex
	//BLACK SCREENS IF NOT GIVEN WEATHER TEX -- TODO: FIND OUT WHY
	glTextureStorage2D(weatherTex, 1, GL_RGBA8, FLUID_SIZE, FLUID_SIZE);
	glTextureSubImage2D(weatherTex, 0, 0, 0, FLUID_SIZE, FLUID_SIZE, GL_RGB, GL_FLOAT, pressureField);
	glBindImageTexture(0, weatherTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);

	//output tex
	glTextureStorage2D(dyeTex, 1, GL_RGBA8, FLUID_SIZE, FLUID_SIZE);
	glBindImageTexture(1, dyeTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glUseProgram(testCompShader);

	glDispatchCompute(1, 128, 1);
	//*/

	RenderMeshes();
}

//performs main fluid computation steps
void Flow2DScene::ComputeFluidImages()
{
	//ADVECTION STEP:
	
	glTextureStorage2D(weatherTex, 1, GL_RGBA8, FLUID_SIZE, FLUID_SIZE);
	glTextureSubImage2D(weatherTex, 0, 0, 0, FLUID_SIZE, FLUID_SIZE, GL_RGB, GL_FLOAT, weatherVelocityField);
	glBindImageTexture(0, weatherTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);

	glTextureStorage2D(pressureTex, 1, GL_RGBA8, PRESSURE_SIZE, PRESSURE_SIZE);
	glTextureSubImage2D(pressureTex, 0, 0, 0, PRESSURE_SIZE, PRESSURE_SIZE, GL_RGB, GL_FLOAT, pressureField);
	glBindImageTexture(1, pressureTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);

	glTextureStorage2D(dyeTex, 1, GL_RGBA8, PRESSURE_SIZE, PRESSURE_SIZE);
	glBindImageTexture(2, dyeTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	
	glUseProgram(advectShader);

	//glUniform1f(0, time.deltaTime);
	//glUniform1f(1, 0.7f);

	glDispatchCompute(1, 128, 1);
	
	//JACOBI ITERATION DIFFUSE STEP:
	
	//TODO
	
	//COMPUTE PRESSURE STEP:
	
	//TODO
	
	//SUBTRACT PRESSURE GRADIENT STEP:
	
	//TODO
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
	glBindTexture(GL_TEXTURE_2D, dyeTex);

	//draw full screen quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
