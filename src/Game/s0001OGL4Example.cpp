#include "s0001OGL4Example.h"
//=============================================================================
namespace
{
	const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}
)";

	const char* fragmentShaderSource = R"(
#version 460 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, TexCoord);
}
)";

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 uv;
	};

	GLuint shaderProgram;
	GLuint VAO, VBO, EBO;
	GLuint texture;
}
//=============================================================================
s0001OGL4Example::~s0001OGL4Example()
{
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
}
//=============================================================================
void s0001OGL4Example::OnStart()
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Вершинные данные

	Vertex vertices[] = {
		// Позиции          // Текстурные координаты
		{{ 0.5f,  0.5f, 0.0f},  {1.0f, 1.0f}}, // Верхний правый
		{{ 0.5f, -0.5f, 0.0f},  {1.0f, 0.0f}}, // Нижний правый
		{{-0.5f, -0.5f, 0.0f},  {0.0f, 0.0f}}, // Нижний левый
		{{-0.5f,  0.5f, 0.0f},  {0.0f, 1.0f}}  // Верхний левый
	};

	unsigned int indices[] = {
		0, 1, 3, // Первый треугольник
		1, 2, 3  // Второй треугольник
	};

	// VAO, VBO, EBO
	glCreateBuffers(1, &VBO);
	glNamedBufferStorage(VBO, sizeof(vertices), vertices, 0);

	glCreateBuffers(1, &EBO);
	glNamedBufferStorage(EBO, sizeof(indices), indices, 0);

	glCreateVertexArrays(1, &VAO);
	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(VAO, EBO);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
	
	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

	// Загрузка текстуры
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("data/temp.png", &width, &height, &nrChannels, 0);

	GLenum internalFormat = (nrChannels == 3) ? GL_RGB8 : GL_RGBA8;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);
	glTextureStorage2D(texture, 1, internalFormat, width, height);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);

	GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
	glTextureSubImage2D(texture, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);

	glGenerateTextureMipmap(texture);

	stbi_image_free(data);
}
//=============================================================================
void s0001OGL4Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
}
//=============================================================================
void s0001OGL4Example::OnUpdate(float deltaTime)
{

}
//=============================================================================
void s0001OGL4Example::OnRender()
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgram);
	glBindTextureUnit(0, texture);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
//=============================================================================
void s0001OGL4Example::OnImGuiDraw()
{

}
//=============================================================================