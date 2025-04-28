#include "s0005OGL3Example.h"
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
	uint32_t    windowWidth{ 0 };
	uint32_t    windowHeight{ 0 };

	GLuint program;
	GLint uWVPLocation;
	GLint uSamplerLocation;

	int cursorLastX;
	int cursorLastY;
	bool firstMouse = true;

	utils::Mesh* mesh;
	utils::CameraTemp* pGameCamera = NULL;
	PersProjInfo persProjInfo;
}
//=============================================================================
s0005OGL3Example::~s0005OGL3Example()
{
	delete pGameCamera;
	delete mesh;
}
//=============================================================================
void s0005OGL3Example::OnStart()
{
	//glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	float FOV = 45.0f;
	float zNear = 1.0f;
	float zFar = 100.0f;

	persProjInfo = { FOV, (float)windowWidth, (float)windowHeight, zNear, zFar };

	program = gl3::CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	uWVPLocation = glGetUniformLocation(program, "uWVP");
	uSamplerLocation = glGetUniformLocation(program, "gSampler");

	Vector3f CameraPos(0.0f, 0.0f, -1.0f);
	Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
	Vector3f CameraUp(0.0f, 1.0f, 0.0f);

	pGameCamera = new utils::CameraTemp(windowWidth, windowHeight, CameraPos, CameraTarget, CameraUp);

	mesh = new utils::Mesh("Data/Mesh/spider.obj");
}
//=============================================================================
void s0005OGL3Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
	screenAspect = (float)width / (float)height;
	windowWidth = width;
	windowHeight = height;
}
//=============================================================================
void s0005OGL3Example::OnMouseMove(int xpos, int ypos)
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

	//camera.Rotate(xoffset, yoffset);
}
//=============================================================================
void s0005OGL3Example::OnUpdate(float deltaTime)
{
	//if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
}
//=============================================================================
void s0005OGL3Example::OnRender()
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	pGameCamera->OnRender();
	float YRotationAngle = 0.1f;

	utils::WorldTrans& worldTransform = mesh->GetWorldTransform();

	worldTransform.SetScale(0.01f);
	worldTransform.SetPosition(0.0f, 0.0f, 2.0f);
	worldTransform.Rotate(0.0f, YRotationAngle, 0.0f);

	Matrix4f World = worldTransform.GetMatrix();
	Matrix4f View = pGameCamera->GetMatrix();

	Matrix4f Projection;
	Projection.InitPersProjTransform(persProjInfo);

	Matrix4f WVP = Projection * View * World;
	glUniformMatrix4fv(uWVPLocation, 1, GL_TRUE, &WVP.m[0][0]);

	mesh->Render();
}
//=============================================================================
void s0005OGL3Example::OnImGuiDraw()
{

}
//=============================================================================