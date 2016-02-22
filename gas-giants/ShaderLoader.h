#pragma once

#include <fstream>
#include <iostream>

class ShaderLoader {
public:
	static std::string ReadShader(const char* fileName);
};