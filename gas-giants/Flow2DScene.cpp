#include "Flow2DScene.h"
#include "ShaderLoader.h"

#include <iostream> //delete later

void Flow2DScene::Update()
{
}

void Flow2DScene::RenderScene()
{
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// An array of 3 vectors which represents 3 vertices
	static const GLfloat vertices[6][2] = {
		{ -0.90, -0.90 },
		{  0.85, -0.90 },
		{ -0.90,  0.85 }, //triangle 1
		{  0.90, -0.85 },
		{  0.90,  0.90 },
		{ -0.85,  0.90 }  //triangle 2
	};

	GLuint BufferID;
	glGenBuffers(1, &BufferID);
	glBindBuffer(GL_ARRAY_BUFFER, BufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	std::string vertProgram = ShaderLoader::ReadShader("BasicShader.vert");
	std::string fragProgram = ShaderLoader::ReadShader("BasicShader.frag");

	printf(vertProgram.c_str());
}
