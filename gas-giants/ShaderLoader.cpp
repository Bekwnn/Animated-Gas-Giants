#include "ShaderLoader.h"

#include <vector>

GLuint ShaderLoader::CompileShaders(const char * vertFileName, const char * fragFileName)
{
	GLint compileResult = GL_FALSE;
	int logLength;
	GLuint vertShaderObject, fragShaderObject;

	//vertex shader: build, compile, link
	std::cout << "Compiling vertex shader..." << std::endl;
	vertShaderObject = glCreateShader(GL_VERTEX_SHADER);
	std::string tempVert = ShaderLoader::ReadShader(vertFileName); //temp used to prevent deletion off stack
	const GLchar* vertProgram = tempVert.c_str();
	glShaderSource(vertShaderObject, 1, &vertProgram, 0);
	glCompileShader(vertShaderObject);

	//vertex shader logging:
	glGetShaderiv(vertShaderObject, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		glGetShaderiv(vertShaderObject, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> vertErrorLog(logLength);
		glGetShaderInfoLog(vertShaderObject, logLength, &logLength, &vertErrorLog[0]);
		std::cout << &vertErrorLog[0] << std::endl;
	}
	else std::cout << "Vertex shader compiled successfully." << std::endl;

	//fragment shader: build, compile, link
	std::cout << "Compiling fragment shader..." << std::endl;
	fragShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	std::string tempFrag = ShaderLoader::ReadShader(fragFileName);
	const GLchar* fragProgram = tempFrag.c_str();
	glShaderSource(fragShaderObject, 1, &fragProgram, 0);
	glCompileShader(fragShaderObject);

	//fragment shader logging:
	glGetShaderiv(fragShaderObject, GL_COMPILE_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		glGetShaderiv(fragShaderObject, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> fragErrorLog(logLength);
		glGetShaderInfoLog(fragShaderObject, logLength, &logLength, &fragErrorLog[0]);
		std::cout << &fragErrorLog[0] << std::endl;
	}
	else std::cout << "Fragment shader compiled successfully." << std::endl;

	//link shaders
	std::cout << "Linking shader program..." << std::endl;
	GLuint shaderObject = glCreateProgram();
	glAttachShader(shaderObject, vertShaderObject);
	glAttachShader(shaderObject, fragShaderObject);
	glLinkProgram(shaderObject);

	//linking logging:
	glGetProgramiv(shaderObject, GL_LINK_STATUS, &compileResult);
	if (compileResult == GL_FALSE)
	{
		glGetProgramiv(shaderObject, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<GLchar> programError(logLength);
		glGetProgramInfoLog(shaderObject, logLength, &logLength, &programError[0]);
		std::cout << &programError[0] << std::endl;
	}
	else std::cout << "Shader program linked successfully." << std::endl;

	//compiled shaders are linked to program, cleanup/delete source
	glDetachShader(shaderObject, vertShaderObject);
	glDetachShader(shaderObject, fragShaderObject);
	glDeleteShader(vertShaderObject);
	glDeleteShader(fragShaderObject);

	return shaderObject;
}

std::string ShaderLoader::ReadShader(const char * fileName)
{
	using namespace std;

	ifstream input(fileName);
	if (!input.good())
	{
		cout << "File failed to load: " << fileName << endl;
		exit(EXIT_FAILURE);
	}

	string fileString = string(
		istreambuf_iterator<char>(input),
		istreambuf_iterator<char>());

	input.close();

	return fileString;
}
