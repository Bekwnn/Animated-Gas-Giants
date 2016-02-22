#include "ShaderLoader.h"

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
