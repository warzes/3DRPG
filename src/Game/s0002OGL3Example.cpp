#include "s0002OGL3Example.h"
//=============================================================================
namespace
{
	const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;

uniform mat4 gTranslate;

void main()
{
	gl_Position = gTranslate * vec4(aPosition, 1.0);
}
)";

	const char* fragmentShaderSource = R"(
#version 330 core

out vec4 FragColor;

void main()
{
	FragColor = vec4(0.3, 0.8, 0.5, 0.0);
}
)";

	GLuint vbo;
	GLuint vao;
	GLuint program;
	GLint translateLocation;

	float scale = 0.0f;
	float delta = 0.8f;
}
//=============================================================================
s0002OGL3Example::~s0002OGL3Example()
{
}
//=============================================================================
void s0002OGL3Example::OnStart()
{
	glm::vec3 vertices[] =
	{
		{-1.0f, -1.0f, 0.0f},
		{ 1.0f, -1.0f, 0.0f},
		{ 0.0f,  1.0f, 0.0f},
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	translateLocation = glGetUniformLocation(program, "gTranslate");
}
//=============================================================================
void s0002OGL3Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
}
//=============================================================================
void s0002OGL3Example::OnUpdate(float deltaTime)
{
	scale += delta * deltaTime;
	if (scale >= 1.0f || scale <= -1.0f)
		delta *= -1.0f;
}
//=============================================================================
void s0002OGL3Example::OnRender()
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);

	// TODO: world = translation * rotation * scaling * positions; == TRSMatrix

	glm::mat4 tr = glm::translate(glm::mat4(1.0f), glm::vec3(scale * 2, scale, 0.0f));
	glUniformMatrix4fv(translateLocation, 1, GL_FALSE, glm::value_ptr(tr));

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
//=============================================================================
void s0002OGL3Example::OnImGuiDraw()
{

}
//=============================================================================