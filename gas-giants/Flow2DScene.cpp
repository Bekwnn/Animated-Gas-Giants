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
	forcesShader = ShaderLoader::CompileCompute("AddForces.comp");
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
			weatherVelocityField[x][y][2] = 0.5f;
		}
	}

	//FLUID VELOCITY FIELD
	//initialize to zero or to weather field
	for (int x = 0; x < FLUID_SIZE; x++)
	{
		for (int y = 0; y < FLUID_SIZE; y++)
		{
			fluidVelocityField[x][y][0] = weatherVelocityField[x][y][0];
			fluidVelocityField[x][y][1] = weatherVelocityField[x][y][1];
			fluidVelocityField[x][y][2] = weatherVelocityField[x][y][2];
		}
	}

	//PRESSURE FIELD
	// holds fluid pressure
	for (int x = 0; x < PRESSURE_SIZE; x++)
	{
		for (int y = 0; y < PRESSURE_SIZE; y++)
		{
			pressureField[x][y][0] = 0.5f;
			pressureField[x][y][1] = 0.5f;
			pressureField[x][y][2] = 0.5f;
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

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &weatherTex);
	glBindTexture(GL_TEXTURE_2D, weatherTex);
	glTextureStorage2D(weatherTex, 1, GL_RGBA8, NOISE_SIZE, NOISE_SIZE);
	glTextureSubImage2D(weatherTex, 0, 0, 0, NOISE_SIZE, NOISE_SIZE, GL_RGB, GL_FLOAT, weatherVelocityField);

	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &fluidVelocityTex);
	glBindTexture(GL_TEXTURE_2D, fluidVelocityTex);
	glTextureStorage2D(fluidVelocityTex, 1, GL_RGBA8, FLUID_SIZE, FLUID_SIZE);
	glTextureSubImage2D(fluidVelocityTex, 0, 0, 0, FLUID_SIZE, FLUID_SIZE, GL_RGB, GL_FLOAT, fluidVelocityField); //change to fluid velocity field later

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &pressureTex);
	glBindTexture(GL_TEXTURE_2D, pressureTex);
	glTextureStorage2D(pressureTex, 1, GL_RGBA8, PRESSURE_SIZE, PRESSURE_SIZE);
	glTextureSubImage2D(pressureTex, 0, 0, 0, PRESSURE_SIZE, PRESSURE_SIZE, GL_RGB, GL_FLOAT, pressureField);

	glActiveTexture(GL_TEXTURE3);
	glGenTextures(1, &dyeTex);
	glBindTexture(GL_TEXTURE_2D, dyeTex);
	glTextureStorage2D(dyeTex, 1, GL_RGBA8, DYE_SIZE, DYE_SIZE);
	glTextureSubImage2D(dyeTex, 0, 0, 0, DYE_SIZE, DYE_SIZE, GL_RGB, GL_FLOAT, dyeField);

	glActiveTexture(GL_TEXTURE4);
	glGenTextures(1, &tempTransferTex);
	glBindTexture(GL_TEXTURE_2D, tempTransferTex);
	glTextureStorage2D(tempTransferTex, 1, GL_RGBA8, DYE_SIZE, DYE_SIZE);
}

void Flow2DScene::Update()
{
}

void Flow2DScene::RenderScene()
{
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ComputeFluidImages();

	RenderMeshes();
}

//performs main fluid computation steps
void Flow2DScene::ComputeFluidImages()
{
	//TODO: reorder steps -> advect should probably be last honestly.

	//ADVECTION STEP:

	// STEP 1.A.) advect dye

	glUseProgram(advectShader);

	//TODO: change weatherTex to fluidVelocityTex later
	// weatherTex should only be needed in add forces step
	glBindImageTexture(0, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, dyeTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(2, tempTransferTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glUniform1f(0, 5.0f*time.deltaTime);

	glDispatchCompute(1, 128, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//copy new dye tex over old dye tex
	glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   dyeTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   FLUID_SIZE, FLUID_SIZE, 1);

	// STEP 1.B.) advect velocity field
	
	glBindImageTexture(0, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(2, tempTransferTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glUniform1f(0, 5.0f*time.deltaTime);

	glDispatchCompute(1, 128, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   fluidVelocityTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   FLUID_SIZE, FLUID_SIZE, 1);
	
	//DIFFUSE STEP:
	/*
	glUseProgram(jacobiShader);

	//diffuse velocity field using jacobi iteration
	
	for (int i = 0; i < 20; i++)
	{
		glBindImageTexture(0, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
		glBindImageTexture(1, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
		glBindImageTexture(2, tempTransferTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

		float alpha = (1.0f / FLUID_SIZE) / time.deltaTime;
		glUniform1f(0, alpha);
		glUniform1f(1, 1.0f/(4.0f + alpha));

		glDispatchCompute(1, 128, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
						   fluidVelocityTex, GL_TEXTURE_2D, 0, 0, 0, 0,
						   FLUID_SIZE, FLUID_SIZE, 1);
	}
	*/
	//diffuse pressure field

	//TODO

	//ADD FORCES STEP:

	///*
	glUseProgram(forcesShader);

	glBindImageTexture(0, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, weatherTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(2, tempTransferTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glUniform1f(0, 5.0f*time.deltaTime);

	glDispatchCompute(1, 128, 1);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   fluidVelocityTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   FLUID_SIZE, FLUID_SIZE, 1);
	//*/
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

	//load texture 1 from GL_TEXTURE0+i
	glUniform1i(glGetUniformLocation(vertFragShader, "tex1"), 1);

	//load texture 2 from GL_TEXTURE0+i
	glUniform1i(glGetUniformLocation(vertFragShader, "tex2"), 3);

	//draw full screen quad
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
