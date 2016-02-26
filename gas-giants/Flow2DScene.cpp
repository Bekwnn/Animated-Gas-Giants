#include "Flow2DScene.h"
#include "ShaderLoader.h"

#include <iostream> //delete later

Flow2DScene::Flow2DScene() : Scene()
{
	shaderObject = ShaderLoader::CompileShaders("BasicShader.vert", "BasicShader.frag");
	CurlNoise curlNoise = CurlNoise();
	for (int x = 0; x < 256; x++)
	{
		for (int y = 0; y < 256; y++)
		{
			glm::vec2 vec = curlNoise.GenerateCurlNoise(x, y);
			restingWeatherTex[x][y][0] = (vec.x+1.f)/2.f;
			restingWeatherTex[x][y][1] = (vec.y+1.f)/2.f;
			restingWeatherTex[x][y][2] = 1.0f;
		}
	}

}

void Flow2DScene::Update()
{
}

void Flow2DScene::RenderScene()
{
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// An array of 3 vectors which represents 3 vertices
	static const GLfloat vertices[6][2] = {
		{ -1.f, -1.f },
		{  1.f, -1.f },
		{ -1.f,  1.f }, //triangle 1
		{  1.f, -1.f },
		{  1.f,  1.f },
		{ -1.f,  1.f }  //triangle 2
	};

	static const GLfloat uvcoords[6][2] = {
		{ 0.f, 0.f },
		{ 1.f, 0.f },
		{ 0.f, 1.f }, //triangle 1
		{ 1.f, 0.f },
		{ 1.f, 1.f },
		{ 0.f, 1.f }  //triangle 2
	};

	static const GLubyte testTex[] = {
		0xFF0000FF,   0x00FF00FF,
		0x0000FFFF,   0xFFFF00FF
	};

	GLuint BufferID;
	glGenBuffers(1, &BufferID);
	glBindBuffer(GL_ARRAY_BUFFER, BufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint UVcoordID;
	glGenBuffers(1, &UVcoordID);
	glBindBuffer(GL_ARRAY_BUFFER, UVcoordID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvcoords), uvcoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glUseProgram(shaderObject);

	GLuint TextureID;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexStorage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2, 2);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 2, 2, GL_RGBA, GL_UNSIGNED_BYTE, testTex);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
