#include "stdafx.h"
#include <fstream>
#include "GLScene.h"
#include "CoreApp.h"

bool GL_LoadShader(GLuint& uiShader, GLenum ShaderType, const GLchar* p_cShader)
{
	// Build and link the shader program
	uiShader = glCreateShader(ShaderType);
	glShaderSource(uiShader, 1, &p_cShader, NULL);
	glCompileShader(uiShader);

	// Check for errors
	GLint iTestReturn;
	glGetShaderiv(uiShader, GL_COMPILE_STATUS, &iTestReturn);
	if (iTestReturn == GL_FALSE) {
		GLchar p_cInfoLog[1024];
		int32_t iErrorLength;
		glGetShaderInfoLog(uiShader, 1024, &iErrorLength, p_cInfoLog);
		Error("Failed to compile shader: " + std::string(p_cInfoLog));
		glDeleteShader(uiShader);
		return false;
	}
	return true;
}

bool GL_LoadShaders(GLuint& uiShader, GLuint uiVertexShader, GLuint uiFragmentShader, GLuint uiGeometryShader, GLuint uiTessControlShader, GLuint uiTessEvalShader)
{
	// Link the shaders
	uiShader = glCreateProgram();
	glAttachShader(uiShader, uiVertexShader);
	glAttachShader(uiShader, uiFragmentShader);
	if (uiGeometryShader != -1) {
		glAttachShader(uiShader, uiGeometryShader);
	}
	if (uiTessControlShader != -1) {
		glAttachShader(uiShader, uiTessControlShader);
		glAttachShader(uiShader, uiTessEvalShader);
	}
	glLinkProgram(uiShader);

	//Check for error in link
	GLint iTestReturn;
	glGetProgramiv(uiShader, GL_LINK_STATUS, &iTestReturn);
	if (iTestReturn == GL_FALSE) {
		GLchar p_cInfoLog[1024];
		int32_t iErrorLength;
		glGetShaderInfoLog(uiShader, 1024, &iErrorLength, p_cInfoLog);
		Error("Failed to link shaders: " + std::string(p_cInfoLog));
		glDeleteProgram(uiShader);
		return false;
	}
	return true;
}

bool GL_LoadShaderFile(GLuint& uiShader, GLenum ShaderType, const char* p_cFileName, int iFileID)
{
	// Открываем файл в режиме чтения
	std::ifstream file(p_cFileName);
	// Используем stringstream для сбора содержимого файла
	std::stringstream buffer;
	buffer << file.rdbuf();
	// Закрываем файл
	file.close();

	// Load in the shader
	bool bReturn = GL_LoadShader(uiShader, ShaderType, buffer.str().c_str());

	// Print the shader name to assist debugging
	if (!bReturn)
		Error(" Failed shader: " + std::string(p_cFileName));

	return bReturn;
}