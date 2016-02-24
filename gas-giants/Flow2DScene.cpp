#include "Flow2DScene.h"
#include "ShaderLoader.h"

#include <iostream> //delete later

Flow2DScene::Flow2DScene() : Scene()
{
	shaderObject = ShaderLoader::CompileShaders("BasicShader.vert", "BasicShader.frag");
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

	GLuint BufferID;
	glGenBuffers(1, &BufferID);
	glBindBuffer(GL_ARRAY_BUFFER, BufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glUseProgram(shaderObject);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
}
