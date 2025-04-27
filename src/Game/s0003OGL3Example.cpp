#include "s0003OGL3Example.h"
//=============================================================================
namespace
{
	const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uWVP;

out vec2 TexCoord;

void main()
{
	gl_Position = uWVP * vec4(aPosition, 1.0);
	TexCoord = aTexCoord;
}
)";

	const char* fragmentShaderSource = R"(
#version 330 core

in vec2 TexCoord;

uniform sampler2D gSampler;

out vec4 FragColor;

void main()
{
	FragColor = texture2D(gSampler, TexCoord);
}
)";

	float screenAspect{ 1.0f };

	GLuint vbo;
	GLuint ibo;
	GLuint vao;
	GLuint program;
	GLint uWorldLocation;
	GLuint texture;

	float scale = 0.0f;
	float delta = 2.0f;

	int cursorLastX;
	int cursorLastY;
	bool firstMouse = true;

	utils::Transform world;

	utils::Camera camera(glm::vec3(0.0f, 0.0f, -2.0f));

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec2 tex;

		Vertex() {}

		Vertex(const glm::vec3& pos_, const glm::vec2& tex_)
		{
			pos = pos_;
			tex = tex_;
		}
	};
}
//=============================================================================
s0003OGL3Example::~s0003OGL3Example()
{
}
//=============================================================================
void s0003OGL3Example::OnStart()
{
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	Vertex Vertices[8];

	glm::vec2 t00 = glm::vec2(0.0f, 0.0f);  // Bottom left
	glm::vec2 t01 = glm::vec2(0.0f, 1.0f);  // Top left
	glm::vec2 t10 = glm::vec2(1.0f, 0.0f);  // Bottom right
	glm::vec2 t11 = glm::vec2(1.0f, 1.0f);  // Top right

	Vertices[0] = Vertex(glm::vec3(0.5f, 0.5f, 0.5f), t00);
	Vertices[1] = Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), t01);
	Vertices[2] = Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), t10);
	Vertices[3] = Vertex(glm::vec3(0.5f, -0.5f, -0.5f), t11);
	Vertices[4] = Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), t00);
	Vertices[5] = Vertex(glm::vec3(0.5f, 0.5f, -0.5f), t10);
	Vertices[6] = Vertex(glm::vec3(0.5f, -0.5f, 0.5f), t01);
	Vertices[7] = Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), t11);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

	program = gl3::CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	uWorldLocation = glGetUniformLocation(program, "uWVP");

	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("data/temp.png", &width, &height, &nrChannels, 0);

	GLenum internalFormat, format;
	if (nrChannels == 1)
	{
		internalFormat = GL_R8;
		format = GL_RED;
	}
	else if (nrChannels == 2)
	{
		internalFormat = GL_RG8;
		format = GL_RG;
	}
	else if (nrChannels == 3)
	{
		internalFormat = GL_RGB8;
		format = GL_RGB;
	}
	else
	{
		internalFormat = GL_RGBA8;
		format = GL_RGBA;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
}
//=============================================================================
void s0003OGL3Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
	screenAspect = (float)width / (float)height;
}
//=============================================================================
void s0003OGL3Example::OnMouseMove(int xpos, int ypos)
{
	if (firstMouse)
	{
		cursorLastX = xpos;
		cursorLastY = ypos;
		firstMouse = false;
	}

	float xoffset = cursorLastX - xpos;
	float yoffset = cursorLastY - ypos;
	cursorLastX = xpos;
	cursorLastY = ypos;

	camera.Rotate(xoffset, yoffset);
}
//=============================================================================
void s0003OGL3Example::OnUpdate(float deltaTime)
{
	scale = delta * deltaTime;

	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
		camera.Move(utils::CameraMovement::Forward, deltaTime);
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		camera.Move(utils::CameraMovement::Backward, deltaTime);
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
		camera.Move(utils::CameraMovement::Left, deltaTime);
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
		camera.Move(utils::CameraMovement::Right, deltaTime);
}
//=============================================================================
void s0003OGL3Example::OnRender()
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);

	world.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
	world.Rotate(glm::vec3(scale, scale, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(65.0f), screenAspect, 0.01f, 1000.0f);
	glm::mat4 mvp = projection * camera.GetViewMatrix() * world.GetWorldMatrix();
	glUniformMatrix4fv(uWorldLocation, 1, GL_FALSE, glm::value_ptr(mvp));

	glActiveTexture(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
//=============================================================================
void s0003OGL3Example::OnImGuiDraw()
{

}
//=============================================================================