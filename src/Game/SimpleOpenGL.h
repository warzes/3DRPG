#pragma once

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

			std::string logError = "OPENGL: Shader compilation failed: ";
			logError += std::string(log);
			logError += ", Source: ";
			logError += shaderSource;

			throw std::exception(logError.c_str());
		}
	}

	inline GLuint CreateShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
	{
		GLuint vertexShader = CreateShader(GL_VERTEX_SHADER, vertexShaderSource);
		GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

		GLuint program = glCreateProgram();
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

			std::string logError = "OPENGL: Shader program linking failed: ";
			logError += std::string(log);
			throw std::exception(logError.c_str());
		}
		return program;
	}

	inline GLuint CreateBuffer(GLenum flags, size_t size, void* data)
	{
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

	inline GLuint CreateTexture2D(GLenum internalFormat, GLenum format, int width, int height, void* data)
	{
		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		glTextureStorage2D(texture, 1, internalFormat, width, height);
		glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

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

		GLenum internalFormat, format;
		if (nrChannels == 1)
		{
			internalFormat = GL_R8;
			format = GL_RED;
		}
		else
		{
			if (nrChannels == 4)
			{
				internalFormat = GL_RGBA8;
				format = GL_RGBA;
			}
			// TODO: 2 компонента
			else
			{
				internalFormat = GL_RGB8;
				format = GL_RGB;
			}
		}

		GLuint texture = CreateTexture2D(internalFormat, format, width, height, data);

		stbi_image_free(data);
		return texture;
	}
}