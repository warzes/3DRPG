#include "stdafx.h"
#include "RHIShaders.h"
#include "Core.h"
//=============================================================================
Shader::Shader(GLenum type, const std::string& source)
	: m_type(type)
{
	m_glShader = glCreateShader(type);
	
	const GLchar* src = source.c_str();
	glShaderSource(m_glShader, 1, &src, nullptr);
	glCompileShader(m_glShader);

	GLint  success;
	glGetShaderiv(m_glShader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE)
	{
		GLchar log[512];

		glGetShaderInfoLog(m_glShader, 512, NULL, log);

		std::string logError = "OPENGL: Shader compilation failed: ";
		logError += std::string(log);
		logError += ", Source: ";
		logError += source;

		Error(logError);
		m_compiled = false;
	}
	else
		m_compiled = true;
}
//=============================================================================
Shader::~Shader()
{
	glDeleteShader(m_glShader);
}
//=============================================================================
Shader::Ptr Shader::Create(GLenum type, const std::string& source)
{
	return std::shared_ptr<Shader>(new Shader(type, source));
}
//=============================================================================
Shader::Ptr Shader::CreateFromFile(GLenum type, const std::string& path, const std::vector<std::string>& defines)
{
	std::string source = ReadShaderCode(path, defines);
	if (source.empty())
	{
		Error("Failed to read GLSL shader source: " + path);
		return nullptr;
	}

	auto shader = Shader::Create(type, source);
	if (shader->Compiled())
		return shader;

	return nullptr;
}
//=============================================================================
GLenum Shader::Type() const
{
	return m_type;
}
//=============================================================================
bool Shader::Compiled() const
{
	return m_compiled;
}
//=============================================================================
GLuint Shader::Id() const
{
	return m_glShader;
}
//=============================================================================
void Shader::SetName(const std::string& name)
{
	setName(m_glShader, name);
}
//=============================================================================
void Shader::setName(const GLuint& name, const std::string& label)
{
	m_name = label;
	glObjectLabel(m_identifier, name, label.size(), label.c_str());
}
//=============================================================================
Program::Program(std::vector<Shader::Ptr> shaders)
{
	if (shaders.size() == 1 && shaders[0]->Type() != GL_COMPUTE_SHADER)
	{
		Fatal("OPENGL: Compute shader programs can only have one shader.");
		return;
	}

	m_glProgram = glCreateProgram();

	for (int i = 0; i < shaders.size(); i++)
		glAttachShader(m_glProgram, shaders[i]->m_glShader);

	glLinkProgram(m_glProgram);

	GLint success;
	glGetProgramiv(m_glProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		char  log[512];
		glGetProgramInfoLog(m_glProgram, 512, NULL, log);

		std::string logError = "OPENGL: Shader program linking failed: ";
		logError += std::string(log);
		Error(logError);
		return;
	}

	int uniformCount = 0;
	glGetProgramiv(m_glProgram, GL_ACTIVE_UNIFORMS, &uniformCount);

	GLint        size;
	GLenum       type;
	GLsizei      length;
	const GLuint bufSize = 64;
	GLchar       name[bufSize];

	for (int i = 0; i < uniformCount; i++)
	{
		glGetActiveUniform(m_glProgram, i, bufSize, &length, &size, &type, name);
		GLuint loc = glGetUniformLocation(m_glProgram, name);

		if (loc != GL_INVALID_INDEX)
			m_locationMap[std::string(name)] = loc;
	}

	glGetProgramiv(m_glProgram, GL_ACTIVE_UNIFORM_BLOCKS, &m_numActiveUniformBlocks);

	for (int i = 0; i < shaders.size(); i++)
		glDetachShader(m_glProgram, shaders[i]->m_glShader);
}
//=============================================================================
Program::~Program()
{
	glDeleteProgram(m_glProgram);
}
//=============================================================================
Program::Ptr Program::Create(std::vector<Shader::Ptr> shaders)
{
	return std::shared_ptr<Program>(new Program(shaders));
}
//=============================================================================
void Program::Use()
{
	glUseProgram(m_glProgram);
}
//=============================================================================
int32_t Program::NumActiveUniformBlocks() const
{
	return m_numActiveUniformBlocks;
}
//=============================================================================
void Program::UniformBlockBinding(std::string name, int binding)
{
	GLuint idx = glGetUniformBlockIndex(m_glProgram, name.c_str());

	if (idx != GL_INVALID_INDEX)
		glUniformBlockBinding(m_glProgram, idx, binding);
}
//=============================================================================
bool Program::SetUniform(std::string name, int32_t value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform1i(m_locationMap[name], value);

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, uint32_t value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform1ui(m_locationMap[name], value);

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, float value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform1f(m_locationMap[name], value);

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, const glm::vec2& value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform2f(m_locationMap[name], value.x, value.y);

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, const glm::vec3& value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform3f(m_locationMap[name], value.x, value.y, value.z);

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, const glm::vec4& value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform4f(m_locationMap[name], value.x, value.y, value.z, value.w);

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, const glm::mat2& value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniformMatrix2fv(m_locationMap[name], 1, GL_FALSE, glm::value_ptr(value));

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, const glm::mat3& value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniformMatrix3fv(m_locationMap[name], 1, GL_FALSE, glm::value_ptr(value));

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, const glm::mat4& value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniformMatrix4fv(m_locationMap[name], 1, GL_FALSE, glm::value_ptr(value));

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, int count, int* value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform1iv(m_locationMap[name], count, value);

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, int count, float* value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform1fv(m_locationMap[name], count, value);

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, int count, glm::vec2* value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform2fv(m_locationMap[name], count, glm::value_ptr(value[0]));

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, int count, glm::vec3* value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform3fv(m_locationMap[name], count, glm::value_ptr(value[0]));

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, int count, glm::vec4* value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniform4fv(m_locationMap[name], count, glm::value_ptr(value[0]));

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, int count, glm::mat2* value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniformMatrix2fv(m_locationMap[name], count, GL_FALSE, glm::value_ptr(value[0]));

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, int count, glm::mat3* value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniformMatrix3fv(m_locationMap[name], count, GL_FALSE, glm::value_ptr(value[0]));

	return true;
}
//=============================================================================
bool Program::SetUniform(std::string name, int count, glm::mat4* value)
{
	if (m_locationMap.find(name) == m_locationMap.end())
		return false;

	glUniformMatrix4fv(m_locationMap[name], count, GL_FALSE, glm::value_ptr(value[0]));

	return true;
}
//=============================================================================
void Program::ExtractReflectionData(ReflectionData& reflectionData)
{
	std::vector<GLchar> nameBuffer;

	int32_t numUniforms = 0;
	glGetProgramInterfaceiv(m_glProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

	for (int i = 0; i < numUniforms; i++)
	{
		GLint results[4];
		glGetProgramResourceiv(m_glProgram, GL_UNIFORM, i, 4, properties, 4, NULL, results);

		if (results[3] == -1)
		{
			nameBuffer.reserve(size_t(results[0] + 1));

			glGetProgramResourceName(m_glProgram, GL_UNIFORM, i, results[0] + 1, NULL, nameBuffer.data());

			std::string name = (const char*)nameBuffer.data();

			switch (results[1])
			{
			case GL_SAMPLER_1D:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_3D:
			case GL_SAMPLER_CUBE:
			case GL_SAMPLER_1D_SHADOW:
			case GL_SAMPLER_2D_SHADOW:
			case GL_SAMPLER_1D_ARRAY:
			case GL_SAMPLER_2D_ARRAY:
			case GL_SAMPLER_1D_ARRAY_SHADOW:
			case GL_SAMPLER_2D_ARRAY_SHADOW:
			case GL_SAMPLER_2D_MULTISAMPLE:
			case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
			case GL_SAMPLER_CUBE_SHADOW:
			case GL_SAMPLER_BUFFER:
			case GL_SAMPLER_2D_RECT:
			case GL_SAMPLER_2D_RECT_SHADOW:
			case GL_INT_SAMPLER_1D:
			case GL_INT_SAMPLER_2D:
			case GL_INT_SAMPLER_3D:
			case GL_INT_SAMPLER_CUBE:
			case GL_INT_SAMPLER_1D_ARRAY:
			case GL_INT_SAMPLER_2D_ARRAY:
			case GL_INT_SAMPLER_2D_MULTISAMPLE:
			case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
			case GL_INT_SAMPLER_BUFFER:
			case GL_INT_SAMPLER_2D_RECT:
			case GL_UNSIGNED_INT_SAMPLER_1D:
			case GL_UNSIGNED_INT_SAMPLER_2D:
			case GL_UNSIGNED_INT_SAMPLER_3D:
			case GL_UNSIGNED_INT_SAMPLER_CUBE:
			case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
			case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
			case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
			case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
			case GL_UNSIGNED_INT_SAMPLER_BUFFER:
			case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
			{
				SamplerReflection reflection;

				reflection.location = results[2];
				reflection.type = results[1];
				reflection.name = name;

				reflectionData.samplers[reflection.location] = reflection;
				break;
			}
			case GL_IMAGE_1D:
			case GL_IMAGE_2D:
			case GL_IMAGE_3D:
			case GL_IMAGE_2D_RECT:
			case GL_IMAGE_CUBE:
			case GL_IMAGE_BUFFER:
			case GL_IMAGE_1D_ARRAY:
			case GL_IMAGE_2D_ARRAY:
			case GL_IMAGE_CUBE_MAP_ARRAY:
			case GL_IMAGE_2D_MULTISAMPLE:
			case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
			case GL_INT_IMAGE_1D:
			case GL_INT_IMAGE_2D:
			case GL_INT_IMAGE_3D:
			case GL_INT_IMAGE_2D_RECT:
			case GL_INT_IMAGE_CUBE:
			case GL_INT_IMAGE_BUFFER:
			case GL_INT_IMAGE_1D_ARRAY:
			case GL_INT_IMAGE_2D_ARRAY:
			case GL_INT_IMAGE_CUBE_MAP_ARRAY:
			case GL_INT_IMAGE_2D_MULTISAMPLE:
			case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
			case GL_UNSIGNED_INT_IMAGE_1D:
			case GL_UNSIGNED_INT_IMAGE_2D:
			case GL_UNSIGNED_INT_IMAGE_3D:
			case GL_UNSIGNED_INT_IMAGE_2D_RECT:
			case GL_UNSIGNED_INT_IMAGE_CUBE:
			case GL_UNSIGNED_INT_IMAGE_BUFFER:
			case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
			case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
			case GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY:
			case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
			case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
			{
				ImageReflection reflection;

				reflection.location = results[2];
				reflection.type = results[1];
				reflection.name = name;

				reflectionData.images[reflection.location] = reflection;
				break;
			}
			default:
			{
				UniformReflection reflection;

				reflection.location = results[2];
				reflection.type = results[1];
				reflection.name = name;

				reflectionData.uniforms[reflection.location] = reflection;
				break;
			}
			}
		}
	}

	int32_t num_ssbos = 0;
	glGetProgramInterfaceiv(m_glProgram, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &num_ssbos);

	GLint ssbo_max_len;
	glGetProgramInterfaceiv(m_glProgram, GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH, &ssbo_max_len);
	nameBuffer.resize(ssbo_max_len);

	for (int i = 0; i < num_ssbos; i++)
	{
		GLsizei strLength;
		glGetProgramResourceName(m_glProgram, GL_SHADER_STORAGE_BLOCK, i, ssbo_max_len, &strLength, nameBuffer.data());

		SSBOReflection reflection;

		reflection.name = (const char*)nameBuffer.data();
		reflection.binding = glGetProgramResourceIndex(m_glProgram, GL_SHADER_STORAGE_BLOCK, reflection.name.c_str());

		reflectionData.ssbos[reflection.binding] = reflection;
	}

	int32_t num_ubos = 0;
	glGetProgramInterfaceiv(m_glProgram, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &num_ubos);

	GLint ubo_max_len;
	glGetProgramInterfaceiv(m_glProgram, GL_UNIFORM_BLOCK, GL_MAX_NAME_LENGTH, &ubo_max_len);
	nameBuffer.resize(ubo_max_len);

	for (int i = 0; i < num_ubos; i++)
	{
		GLsizei strLength;
		glGetProgramResourceName(m_glProgram, GL_UNIFORM_BLOCK, i, ubo_max_len, &strLength, nameBuffer.data());

		UBOReflection reflection;

		reflection.name = (const char*)nameBuffer.data();
		reflection.binding = glGetProgramResourceIndex(m_glProgram, GL_UNIFORM_BLOCK, reflection.name.c_str());

		reflectionData.ubos[reflection.binding] = reflection;
	}
}
//=============================================================================
GLint Program::Id() const
{
	return m_glProgram;
}
//=============================================================================
void Program::SetName(const std::string& name)
{
	setName(m_glProgram, name);
}
//=============================================================================
void Program::setName(const GLuint& name, const std::string& label)
{
	m_name = label;
	glObjectLabel(m_identifier, name, label.size(), label.c_str());
}
//=============================================================================