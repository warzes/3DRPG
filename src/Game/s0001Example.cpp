#include "s0001Example.h"
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

	GLuint shaderProgram;
	GLuint VAO, VBO, EBO;
	GLuint texture;
}
//=============================================================================
s0001Example::~s0001Example()
{
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);
}
//=============================================================================
void s0001Example::OnStart()
{
	// Компиляция шейдеров
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	// Программа шейдеров
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Вершинные данные
	float vertices[] = {
		// Позиции          // Текстурные координаты
		 0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // Верхний правый
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Нижний правый
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // Нижний левый
		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // Верхний левый
	};

	unsigned int indices[] = {
		0, 1, 3, // Первый треугольник
		1, 2, 3  // Второй треугольник
	};

	// VAO, VBO, EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// Загрузка текстуры
	int width, height, nrChannels;
	unsigned char* data = stbi_load("data/temp.png", &width, &height, &nrChannels, 4);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

}
//=============================================================================
void s0001Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
}
//=============================================================================
void s0001Example::OnUpdate(float deltaTime)
{

}
//=============================================================================
void s0001Example::OnRender()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgram);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
//=============================================================================
void s0001Example::OnImGuiDraw()
{

}
//=============================================================================