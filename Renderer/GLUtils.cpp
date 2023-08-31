#include "pch.h"
#include "GLUtils.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

GLuint GLUtils::CreateProgram(const char* vertexPath, const char* pixelPath)
{
	// Create the shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint pixelShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertexPath, std::ios::in);
	if (vertexShaderStream.is_open()) 
	{
		std::stringstream sstr;
		sstr << vertexShaderStream.rdbuf();
		vertexShaderCode = sstr.str();
		vertexShaderStream.close();
	}
	else 
	{
		std::cout << "Impossible to open " << vertexPath << ".Are you in the right directory ? Don't forget to read the FAQ !" << std::endl;
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string pixelShaderCode;
	std::ifstream pixelShaderStream(pixelPath, std::ios::in);
	if (pixelShaderStream.is_open()) 
	{
		std::stringstream sstr;
		sstr << pixelShaderStream.rdbuf();
		pixelShaderCode = sstr.str();
		pixelShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	std::cout << "Compiling shader : " << vertexPath << std::endl;
	char const* vertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
	glCompileShader(vertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) 
	{
		std::vector<char> vertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(vertexShaderID, InfoLogLength, NULL, &vertexShaderErrorMessage[0]);
		printf("%s\n", &vertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", pixelPath);
	char const* pixelSourcePointer = pixelShaderCode.c_str();
	glShaderSource(pixelShaderID, 1, &pixelSourcePointer, NULL);
	glCompileShader(pixelShaderID);

	// Check Fragment Shader
	glGetShaderiv(pixelShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(pixelShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> pixelShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(pixelShaderID, InfoLogLength, NULL, &pixelShaderErrorMessage[0]);
		printf("%s\n", &pixelShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, pixelShaderID);
	glLinkProgram(programID);

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> programErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &programErrorMessage[0]);
		printf("%s\n", &programErrorMessage[0]);
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, pixelShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(pixelShaderID);

	return programID;
}