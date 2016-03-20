#include "Flow2DScene.h"
#include "ShaderLoader.h"
#include "RandomColorGrad.h"

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
	glm::vec2 leftband(-0.8f, 0.0f);
	glm::vec2 rightband(0.8f, 0.0f);
	for (int x = 0; x < NOISE_SIZE; x++)
	{
		for (int y = 0; y < NOISE_SIZE; y++)
		{
			glm::vec2 resultvec;
			glm::vec2 noisevec = curlNoise.GenerateCurlNoise(x, y, 16);
			float twolerpval = cosf(2.0f*3.1415f*(x / ((float)NOISE_SIZE - 1.0f))*4.0f);
			if (twolerpval < 0.0f)
				resultvec = glm::mix(leftband, noisevec, twolerpval/2.f + 1.0f);
			else
				resultvec = glm::mix(noisevec, rightband, twolerpval/2.f);

			weatherVelocityField[x][y][0] = (resultvec.x + 1.f)/2.f;
			weatherVelocityField[x][y][1] = (resultvec.y + 1.f)/2.f;
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
			pressureField[x][y][1] = 0.f;
			pressureField[x][y][2] = 0.f;
		}
	}

	//DYE FIELD
	// holds the actual colors of the simulation,
	// initialize to whatever pattern the planet should have

	//create random height bands near a desired color
	RandomColorGrad band1(0.1608f, 0.6706f, 0.5294f);
	RandomColorGrad band2(0.3216f, 0.8627f, 0.3922f);

	glm::vec3 curband1 = band1.GetColorNear(0.1f);
	int curBandRun1 = (rand() % 20) + 1; //height 1-8 pixel bands

	glm::vec3 curband2 = band2.GetColorNear(0.1f);
	int curBandRun2 = (rand() % 20) + 1;

	for (int x = 0; x < DYE_SIZE; x++)
	{
		float coslerpval = cosf(2.0f*3.1415f*(x / ((float)NOISE_SIZE - 1.0f))*4.0f);
		if (coslerpval > 0.f)
		{
			for (int y = 0; y < DYE_SIZE; y++)
			{
				dyeField[x][y][0] = curband1.r;
				dyeField[x][y][1] = curband1.g;
				dyeField[x][y][2] = curband1.b;
			}
			curBandRun1--;
		}
		else if (coslerpval <= 0.f)
		{
			for (int y = 0; y < DYE_SIZE; y++)
			{
				dyeField[x][y][0] = curband2.r;
				dyeField[x][y][1] = curband2.g;
				dyeField[x][y][2] = curband2.b;
			}
			curBandRun2--;
		}

		if (curBandRun1 <= 0)
		{
			curband1 = band1.GetColorNear(0.1f);
			curBandRun1 = (rand() % 20) + 1; //height 1-8 pixel bands
		}
		if (curBandRun2 <= 0)
		{
			curband2 = band2.GetColorNear(0.1f);
			curBandRun2 = (rand() % 20) + 1;
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
	glUseProgram(advectShader);

	// STEP 1.A.) advect dye
	glBindImageTexture(0, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, dyeTex,           0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(2, tempTransferTex,  0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glUniform1f(0, time.deltaTime);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glDispatchCompute(1, 512, 1);

	//copy new dye tex over old dye tex
	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
	glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   dyeTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   FLUID_SIZE, FLUID_SIZE, 1);

	// STEP 1.B.) advect velocity field
	glBindImageTexture(0, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(2, tempTransferTex,  0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glUniform1f(0, time.deltaTime);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glDispatchCompute(1, 512, 1);

	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
	glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   fluidVelocityTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   FLUID_SIZE, FLUID_SIZE, 1);
	
	//VISCOUS DIFFUSE STEP:
	glUseProgram(jacobiShader);

	for (int i = 0; i < 50; i++)
	{
		glBindImageTexture(0, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
		glBindImageTexture(1, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
		glBindImageTexture(2, tempTransferTex,  0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

		float viscocity = 0.3f;
		float alpha = (1.0f / (FLUID_SIZE*FLUID_SIZE)) / (viscocity*time.deltaTime);
		glUniform1f(0, alpha);
		glUniform1f(1, 1.0f/(4.0f + alpha)); //reciprocal of beta

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glDispatchCompute(1, 512, 1);

		glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
		glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
						   fluidVelocityTex, GL_TEXTURE_2D, 0, 0, 0, 0,
						   FLUID_SIZE, FLUID_SIZE, 1);
	}

	//ADD FORCES STEP:
	glUseProgram(forcesShader);

	glBindImageTexture(0, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, weatherTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(2, tempTransferTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glUniform1f(0, 200.0f*time.deltaTime);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glDispatchCompute(1, 512, 1);

	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
	glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   fluidVelocityTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   FLUID_SIZE, FLUID_SIZE, 1);


	//COMPUTE DIVERGENCE STEP:
	glUseProgram(divergenceShader);

	glBindImageTexture(0, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, tempTransferTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glDispatchCompute(1, 512, 1);

	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
	glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   divergenceTex, GL_TEXTURE_2D, 0, 0, 0, 0,
	                   FLUID_SIZE, FLUID_SIZE, 1);

	//POISSON-PRESSURE EQUATION STEP:
	///*
	glUseProgram(jacobiShader);

	//TODO: zero out pressure tex (before loop); we use that as initial guess

	for (int i = 0; i < 80; i++)
	{
		glBindImageTexture(0, pressureTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
		glBindImageTexture(1, divergenceTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
		glBindImageTexture(2, tempTransferTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

		float alpha = -(1.0f / (FLUID_SIZE*FLUID_SIZE));
		glUniform1f(0, alpha);
		glUniform1f(1, 1.0f/4.0f);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glDispatchCompute(1, 512, 1);

		glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
		glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
		                   pressureTex, GL_TEXTURE_2D, 0, 0, 0, 0,
		                   FLUID_SIZE, FLUID_SIZE, 1);
	}
	//*/
	 
	//SUBTRACT PRESSURE GRADIENT STEP:
	glUseProgram(gradientShader);

	glBindImageTexture(0, fluidVelocityTex, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
	glBindImageTexture(1, pressureTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	glBindImageTexture(2, tempTransferTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glDispatchCompute(1, 512, 1);

	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT);
	glCopyImageSubData(tempTransferTex, GL_TEXTURE_2D, 0, 0, 0, 0,
		fluidVelocityTex, GL_TEXTURE_2D, 0, 0, 0, 0,
		FLUID_SIZE, FLUID_SIZE, 1);

	//TODO move dye advection to last step for system stability
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
