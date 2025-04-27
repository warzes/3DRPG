#include "s0002OGL3Example.h"
//=============================================================================
namespace
{
	const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

uniform mat4 uWorld;

out vec4 Color;

void main()
{
	gl_Position = uWorld * vec4(aPosition, 1.0);
	Color = vec4(aColor, 1.0);
}
)";

	const char* fragmentShaderSource = R"(
#version 330 core

in vec4 Color;

out vec4 FragColor;

void main()
{
	FragColor = Color;
}
)";

	float screenAspect{ 1.0f };

	GLuint vbo;
	GLuint ibo;
	GLuint vao;
	GLuint program;
	GLint uWorldLocation;

	float scale = 0.0f;
	float delta = 1.4f;

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;

		Vertex() {}

		Vertex(float x, float y, float z)
		{
			pos = glm::vec3(x, y, z);

			float red = (float)rand() / (float)RAND_MAX;
			float green = (float)rand() / (float)RAND_MAX;
			float blue = (float)rand() / (float)RAND_MAX;
			color = glm::vec3(red, green, blue);
		}
	};
}
//=============================================================================
s0002OGL3Example::~s0002OGL3Example()
{
}
//=============================================================================
void s0002OGL3Example::OnStart()
{
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	Vertex Vertices[8];

	Vertices[0] = Vertex(0.5f, 0.5f, 0.5f);
	Vertices[1] = Vertex(-0.5f, 0.5f, -0.5f);
	Vertices[2] = Vertex(-0.5f, 0.5f, 0.5f);
	Vertices[3] = Vertex(0.5f, -0.5f, -0.5f);
	Vertices[4] = Vertex(-0.5f, -0.5f, -0.5f);
	Vertices[5] = Vertex(0.5f, 0.5f, -0.5f);
	Vertices[6] = Vertex(0.5f, -0.5f, 0.5f);
	Vertices[7] = Vertex(-0.5f, -0.5f, 0.5f);

	vbo = gl3::CreateVertexBuffer(GL_STATIC_DRAW, sizeof(Vertices), Vertices);

	unsigned int Indices[] = {
							 0, 1, 2,
							 1, 3, 4,
							 5, 6, 3,
							 7, 3, 6,
							 2, 4, 7,
							 0, 7, 6,
							 0, 5, 1,
							 1, 5, 3,
							 5, 0, 6,
							 7, 4, 3,
							 2, 1, 4,
							 0, 2, 7
	};

	ibo = gl3::CreateIndexBuffer(GL_STATIC_DRAW, sizeof(Indices), Indices);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	program = gl3::CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	uWorldLocation = glGetUniformLocation(program, "uWorld");
}
//=============================================================================
void s0002OGL3Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
	screenAspect = (float)width / (float)height;
}
//=============================================================================
void s0002OGL3Example::OnMouseMove(int xpos, int ypos)
{
}
//=============================================================================
void s0002OGL3Example::OnUpdate(float deltaTime)
{
	scale += delta * deltaTime;
}
//=============================================================================
void s0002OGL3Example::OnRender()
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);

	// TODO: world = translation * rotation * scaling * positions; == TRSMatrix

	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), scale, glm::vec3(1.0, 1.0, 0.0));
	glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f));
	glm::mat4 projection = glm::perspective(glm::radians(65.0f), screenAspect, 0.01f, 1000.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 world = projection * view * translateMat * rotateMat * scaleMat;

	glUniformMatrix4fv(uWorldLocation, 1, GL_FALSE, glm::value_ptr(world));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
//=============================================================================
void s0002OGL3Example::OnImGuiDraw()
{

}
//=============================================================================