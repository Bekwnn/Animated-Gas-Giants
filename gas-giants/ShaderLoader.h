#pragma once
#include <fstream>
#include <iostream>

#include <GL\glew.h>

class ShaderLoader {
public:
	static GLuint CompileShaders(const char* vertFileName, const char* fragFileName);
	static std::string ReadShader(const char* fileName);
};