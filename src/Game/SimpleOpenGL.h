#pragma once

#include "SimpleOpenGLUtils.h"

namespace sample
{
	inline GLuint CreateShader(GLenum type, const char* shaderSource)
	{
		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &shaderSource, nullptr);
		glCompileShader(shader);

		GLint  success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLchar log[512];
			glGetShaderInfoLog(shader, 512, nullptr, log);

			const std::string logError 
				= "OPENGL " + ShaderTypeToString(type) + ": Shader compilation failed : " 
				+ std::string(log) + ", Source: \n" + shaderSource;
			throw std::exception(logError.c_str());
		}

		return shader;
	}

	inline GLuint CreateShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
	{
		GLuint vertexShader   = CreateShader(GL_VERTEX_SHADER, vertexShaderSource);
		GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
		GLuint program        = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		GLint success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			char  log[512];
			glGetProgramInfoLog(program, 512, nullptr, log);
			std::string logError = "OPENGL: Shader program linking failed: " + std::string(log);
			throw std::exception(logError.c_str());
		}
		return program;
	}

	inline GLuint CreateBuffer(GLenum flags, size_t size, void* data)
	{
		/*
		flags:
			GL_DYNAMIC_STORAGE_BIT - содержимое можно будет изменять через glBufferSubData
			GL_MAP_READ_BIT - можно мапить для чтения
			GL_MAP_READ_BIT - можно мапить для записи
			GL_MAP_PERSISTENT_BIT - запрос на чтение или запись через мапинг. Указатель на данные действителен пока выполняется мапинг, даже во время выполнения команд отрисовки. Должен быть минимум с GL_MAP_READ_BIT или GL_MAP_READ_BIT
			GL_MAP_COHERENT_BIT - соответствие данных клиента и сервера без дополнительных действий. должен быть с GL_MAP_PERSISTENT_BIT
			GL_CLIENT_STORAGE_BIT - 
		*/
		GLuint buffer;
		glCreateBuffers(1, &buffer);
		glNamedBufferStorage(buffer, size, data, flags);
		return buffer;
	}

	// example:
	//	SetVertexAttrib(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
	//	SetVertexAttrib(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	inline void SetVertexAttrib(GLuint vao, GLuint attribIndex, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset)
	{
		glEnableVertexArrayAttrib(vao, attribIndex);
		glVertexArrayAttribBinding(vao, attribIndex, 0);
		glVertexArrayAttribFormat(vao, attribIndex, size, type, normalized, relativeOffset);
	}

	// TODO: TextureParameter
	inline GLuint CreateTexture2D(GLenum internalFormat, int width, int height, void* data)
	{
		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);

		glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureStorage2D(texture, 1, internalFormat, width, height);
		glTextureSubImage2D(texture, 0, 0, 0, width, height, GetBaseFormat(internalFormat), GL_UNSIGNED_BYTE, data);

		glGenerateTextureMipmap(texture);

		return texture;
	}

	inline GLuint LoadTexture2D(const char* texturePath, bool flipVertical = false)
	{
		stbi_set_flip_vertically_on_load(flipVertical);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
		if (!data)
			throw std::exception((std::string("Texture: ") + texturePath + " not find").c_str());

		GLenum internalFormat;
		if (nrChannels == 1) internalFormat = GL_R8;
		else if (nrChannels == 2) internalFormat = GL_RG8;
		else if (nrChannels == 3) internalFormat = GL_RGB8;
		else internalFormat = GL_RGBA8;

		GLuint texture = CreateTexture2D(internalFormat, width, height, data);

		stbi_image_free(data);
		return texture;
	}
}