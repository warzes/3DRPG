#include "stdafx.h"
#include "GameApp.h"
//=============================================================================
// Shader sources
#pragma region [ Shaders sources ]
const GLchar* vertexShaderSource = R"glsl(
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPosition, 1.0);
	Normal = aNormal;
	TexCoords = aTexCoords;
}
)glsl";

const GLchar* fragmentShaderSource = R"glsl(
#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D textureDiffuse;

vec3 lightPos = vec3(4.0, 3.0, 6.0);
vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main()
{
	float lightAngle = max(dot(normalize(Normal), normalize(lightPos)), 0.0);
	FragColor = texture(textureDiffuse, TexCoords) * vec4((0.3 + 0.7 * lightAngle) * lightColor, 1.0);
}
)glsl";

#pragma endregion
//=============================================================================
std::shared_ptr<ShaderProgram> shader;
std::shared_ptr<Material> tempMaterial;
std::shared_ptr<Model> model;
std::shared_ptr<Model> modelCube;
std::shared_ptr<Model> modelSphere;
std::shared_ptr<Model> modelPlane;



Camera camera;
Scene scene;
Node node;
Node nodeCube;
Node nodeSphere;
Node nodePlane;

bool firstMouse = true;
float lastX = 1600.0f / 2.0;
float lastY = 900.0f / 2.0;
//=============================================================================
bool InitGame()
{
	rhi::Init();

	shader = std::make_shared<ShaderProgram>(vertexShaderSource, fragmentShaderSource);
	tempMaterial = std::make_shared<Material>(Texture2D::LoadFromFile("data/temp.png"));
	//model = std::make_shared<Model>("data/cube.obj", tempMaterial);
	model = std::make_shared<Model>("data/treeRealistic/Tree.obj");

	modelCube = Model::CreateCube(1);
	modelSphere = Model::CreateSphere(1.0f, 36, 18);
	modelPlane = Model::CreatePlane(10.0f, 10.0f, 4.0f, 4.0f);

	nodePlane.SetModel(modelPlane);
	scene.AddNode(&nodePlane);

	nodeCube.SetModel(modelCube);
	nodeCube.GetTransform().SetPosition(glm::vec3(2.0f, 0.0f, -5.0f));
	scene.AddNode(&nodeCube);

	node.SetModel(model);
	node.GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	node.GetTransform().Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));	
	scene.AddNode(&node);


	scene.SetModelLocation(glGetUniformLocation(shader->GetID(), "model"));


	return true;
}
//=============================================================================
void CloseGame()
{
	ClearDefaultGraphicsResource();
	rhi::Close();
}
//=============================================================================
void FixedUpdate(double deltaTime)
{
}
//=============================================================================
void FrameGame(double deltaTime)
{
	ProcessInput(camera, deltaTime, firstMouse, lastX, lastY);

	glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->Bind();
	glm::mat4 projection = camera.GetProjectionMatrix(GetFrameAspect());
	glm::mat4 view = camera.GetViewMatrix();
	glProgramUniformMatrix4fv(shader->GetID(), glGetUniformLocation(shader->GetID(), "projection"), 1, GL_FALSE, &projection[0][0]);
	glProgramUniformMatrix4fv(shader->GetID(), glGetUniformLocation(shader->GetID(), "view"), 1, GL_FALSE, &view[0][0]);

	scene.Render(shader, camera, GetFrameAspect());
}
//=============================================================================
void DrawImGui(double deltaTime)
{

}
//=============================================================================
void ProcessInput(Camera& camera, float deltaTime, bool& firstMouse, float& lastX, float& lastY)
{
	if (glfwGetMouseButton(GetWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		double xpos, ypos;
		glfwGetCursorPos(GetWindow(), &xpos, &ypos);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else 
	{
		glfwSetInputMode(GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstMouse = true;
	}

	if (glfwGetKey(GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Direction::Forward, deltaTime);
	if (glfwGetKey(GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Direction::Backward, deltaTime);
	if (glfwGetKey(GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Direction::Left, deltaTime);
	if (glfwGetKey(GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Direction::Right, deltaTime);
}
//=============================================================================