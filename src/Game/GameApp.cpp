#include "stdafx.h"
#include "GameApp.h"
//=============================================================================
// Shader sources
#pragma region [ Shaders sources ]
const GLchar* vertexShaderSource = R"glsl(
#version 430 core

layout(binding = 0) uniform TransformData
{
	mat4 model;
};

layout(binding = 1) uniform CameraData
{
	mat4 view;
	mat4 projection;
	vec3 cameraPosition;
};

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoords;

layout(location = 0) smooth out vec3 PositionOut;
layout(location = 1) smooth out vec3 NormalOut;
layout(location = 2) smooth out vec2 TexCoordsOut;

void main()
{
	// Transform vertex
	vec4 position = model * vec4(VertexPosition, 1.0f);
	gl_Position = projection * view * position;
	PositionOut = position.xyz;

	// Transform normal
	vec4 normal = model * vec4(VertexNormal, 0.0f);
	NormalOut = normal.xyz;

	// Pass-through UV coordinates
	TexCoordsOut = VertexTexCoords;
}
)glsl";

//const GLchar* fragmentShaderSource = R"glsl(
//#version 430 core
//
//layout(binding = 1) uniform CameraData
//{
//	mat4 view;
//	mat4 projection;
//	vec3 cameraPosition;
//};
//
//layout(location = 0) in vec3 PositionIn;
//layout(location = 1) in vec3 NormalIn;
//layout(location = 2) in vec2 TexCoordsIn;
//
//out vec4 FragColorOut;
//
//uniform sampler2D textureDiffuse;
//
//vec3 lightPos = vec3(4.0, 3.0, 6.0);
//vec3 lightColor = vec3(1.0, 1.0, 1.0);
//
//void main()
//{
//	float lightAngle = max(dot(normalize(NormalIn), normalize(lightPos)), 0.0);
//	FragColorOut = texture(textureDiffuse, TexCoordsIn) * vec4((0.3 + 0.7 * lightAngle) * lightColor, 1.0);
//}
//)glsl";

const GLchar* fragmentShaderSource = R"glsl(
#version 430 core

struct PointLight
{
	vec3 v3LightPosition;
	vec3 v3LightIntensity;
	vec3 v3Falloff;
};

#define MAX_LIGHTS 16

layout(binding = 1) uniform CameraData
{
	mat4 view;
	mat4 projection;
	vec3 cameraPosition;
};

layout(std140, binding = 2) uniform PointLightData
{
	PointLight PointLights[MAX_LIGHTS];
};

layout(location = 0) uniform int iNumPointLights;

layout(binding = 0) uniform sampler2D s2DiffuseTexture;
layout(binding = 1) uniform sampler2D s2SpecularTexture;
layout(binding = 2) uniform sampler2D s2RoughnessTexture;

//layout(std140, binding = 3) uniform MaterialData
//{
//	vec3  v3DiffuseColour;
//	vec3  v3SpecularColour;
//	float fRoughness;
//};

#define M_RCPPI 0.31830988618379067153776752674503f
#define M_PI 3.1415926535897932384626433832795f

layout(location = 0) in vec3 PositionIn;
layout(location = 1) in vec3 NormalIn;
layout(location = 2) in vec2 TexCoordsIn;

out vec4 FragColorOut;

vec3 lightFalloff(in vec3 v3LightIntensity, in vec3 v3Falloff, in vec3 v3LightPosition, in vec3 v3Position)
{
	// Calculate distance from light
	float fDist = distance(v3LightPosition, v3Position);

	// Return falloff
	float fFalloff = v3Falloff.x + (v3Falloff.y * fDist) + (v3Falloff.z * fDist * fDist);
	return v3LightIntensity / fFalloff;
}

vec3 schlickFresnel(in vec3 v3LightDirection, in vec3 v3Normal, in vec3 v3SpecularColour)
{
	// Schlick Fresnel approximation
	float fLH = dot(v3LightDirection, v3Normal);
	return v3SpecularColour + (1.0f - v3SpecularColour) * pow(1.0f - fLH, 5);
}

float TRDistribution(in vec3 v3Normal, in vec3 v3HalfVector, in float fRoughness)
{
	// Trowbridge-Reitz Distribution function
	float fNSq = fRoughness * fRoughness;
	float fNH = max(dot(v3Normal, v3HalfVector), 0.0f);
	float fDenom = fNH * fNH * (fNSq - 1.0f) + 1.0f;
	return fNSq / (M_PI * fDenom * fDenom);
}

float GGXVisibility(in vec3 v3Normal, in vec3 v3LightDirection, in vec3 v3ViewDirection, in float fRoughness)
{
	// GGX Visibility function
	float fNL = max(dot(v3Normal, v3LightDirection), 0.0f);
	float fNV = max(dot(v3Normal, v3ViewDirection), 0.0f);
	float fRSq = fRoughness * fRoughness;
	float fRMod = 1.0f - fRSq;
	float fRecipG1 = fNL + sqrt(fRSq + (fRMod * fNL * fNL));
	float fRecipG2 = fNV + sqrt(fRSq + (fRMod * fNV * fNV));

	return 1.0f / (fRecipG1 * fRecipG2);
}

subroutine vec3 BRDF(in vec3, in vec3, in vec3, in vec3, in vec3, in vec3, in float);
layout(location = 0) subroutine uniform BRDF BRDFUniform;

layout(index = 0) subroutine(BRDF) vec3 GGX(in vec3 v3Normal, in vec3 v3LightDirection, in vec3 v3ViewDirection, in vec3 v3LightIrradiance, in vec3 v3DiffuseColour, in vec3 v3SpecularColour, in float fRoughness)
{
	// Calculate diffuse component
	vec3 v3Diffuse = v3DiffuseColour * M_RCPPI;

	// Calculate half vector
	vec3 v3HalfVector = normalize(v3ViewDirection + v3LightDirection);

	// Calculate Toorance-Sparrow components
	vec3 v3F = schlickFresnel(v3LightDirection, v3HalfVector, v3SpecularColour);
	float fD = TRDistribution(v3Normal, v3HalfVector, fRoughness);
	float fV = GGXVisibility(v3Normal, v3LightDirection, v3ViewDirection, fRoughness);

	// Modify diffuse by Fresnel reflection
	v3Diffuse *= (1.0f - v3F);

	// Combine diffuse and specular
	vec3 v3RetColour = v3Diffuse + (v3F * fD * fV);

	// Multiply by view angle
	v3RetColour *= max(dot(v3Normal, v3LightDirection), 0.0f);

	// Combine with incoming light value
	v3RetColour *= v3LightIrradiance;

	return v3RetColour;
}

layout(index = 1) subroutine(BRDF) vec3 blinnPhong(in vec3 v3Normal, in vec3 v3LightDirection, in vec3 v3ViewDirection, in vec3 v3LightIrradiance, in vec3 v3DiffuseColour, in vec3 v3SpecularColour, in float fRoughness)
{
	// Get diffuse component
	vec3 v3Diffuse = v3DiffuseColour;

	// Calculate half vector
	vec3 v3HalfVector = normalize(v3ViewDirection + v3LightDirection);

	// Convert roughness to Phong shininess
	float fRoughnessPhong = (2.0f / (fRoughness * fRoughness)) - 2.0f;

	// Calculate specular component
	vec3 v3Specular = pow(max(dot(v3Normal, v3HalfVector), 0.0f), fRoughnessPhong) * v3SpecularColour;

	// Normalise diffuse and specular component and add
	v3Diffuse *= M_RCPPI;
	v3Specular *= (fRoughnessPhong + 8.0f) / (8.0f * M_PI);

	// Combine diffuse and specular
	vec3 v3RetColour = v3Diffuse + v3Specular;

	// Multiply by view angle
	v3RetColour *= max(dot(v3Normal, v3LightDirection), 0.0f);

	// Combine with incoming light value
	v3RetColour *= v3LightIrradiance;

	return v3RetColour;
}

void main()
{
	// Normalize the inputs
	vec3 v3Normal = normalize(NormalIn);
	vec3 v3ViewDirection = normalize(cameraPosition - PositionIn);

	// Get texture data
	vec4 DiffuseColour = texture(s2DiffuseTexture, TexCoordsIn); // TODO: add mat
	vec3 v3SpecularColour = texture(s2SpecularTexture, TexCoordsIn).rgb; // TODO: add mat
	float fRoughness = texture(s2RoughnessTexture, TexCoordsIn).r; // TODO: add mat

	// Loop over each point light
	vec3 v3RetColour = vec3(0.0f);
	for (int i = 0; i < iNumPointLights; i++)
	{
		vec3 v3LightDirection = normalize(PointLights[i].v3LightPosition - PositionIn);

		// Calculate light falloff
		vec3 v3LightIrradiance = lightFalloff(PointLights[i].v3LightIntensity, PointLights[i].v3Falloff, PointLights[i].v3LightPosition, PositionIn);

		// Perform shading
		v3RetColour += BRDFUniform(v3Normal, v3LightDirection, v3ViewDirection, v3LightIrradiance, DiffuseColour.rgb, v3SpecularColour, fRoughness);
	}

	// Add in ambient contribution
	v3RetColour += DiffuseColour.rgb * vec3(0.3f);
	FragColorOut = vec4(v3RetColour, DiffuseColour.a);
}
)glsl";

#pragma endregion
//=============================================================================
std::shared_ptr<ShaderProgram> shader;
std::shared_ptr<Material> tempMaterial;
std::shared_ptr<Model> model;
std::shared_ptr<Model> modelCathedral;
std::shared_ptr<Model> modelCube;
std::shared_ptr<Model> modelSphere;
std::shared_ptr<Model> modelPlane;

Camera camera;
Scene scene;
Node node;
Node nodeCathedral;
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

	scene.Init();

	shader = std::make_shared<ShaderProgram>(vertexShaderSource, fragmentShaderSource);
	tempMaterial = std::make_shared<Material>(
		Texture2D::LoadFromFile("data/Textures/CrateDiffuse.bmp"),
		Texture2D::LoadFromFile("data/Textures/CrateSpecular.bmp"),
		Texture2D::LoadFromFile("data/Textures/CrateRoughness.bmp")
		);

	//model = std::make_shared<Model>("data/cube.obj", tempMaterial);
	model = std::make_shared<Model>("data/treeRealistic/Tree.obj");
	modelCathedral = std::make_shared<Model>("data/Cathedral/TutorialCathedral.fbx");

	modelCube = Model::CreateCube(1, tempMaterial);
	modelSphere = Model::CreateSphere(1.0f, 36, 18, tempMaterial);
	modelPlane = Model::CreatePlane(10.0f, 10.0f, 4.0f, 4.0f, tempMaterial);

	nodePlane.SetModel(modelPlane);
	//scene.AddNode(&nodePlane);

	nodeSphere.SetModel(modelSphere);
	nodeSphere.GetTransform().SetPosition(glm::vec3(-2.0f, 0.0f, -5.0f));
	//scene.AddNode(&nodeSphere);

	nodeCube.SetModel(modelCube);
	nodeCube.GetTransform().SetPosition(glm::vec3(2.0f, 0.0f, -5.0f));
	//scene.AddNode(&nodeCube);

	node.SetModel(model);
	node.GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	node.GetTransform().Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	//scene.AddNode(&node);

	nodeCathedral.SetModel(modelCathedral);
	nodeCathedral.GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	scene.AddNode(&nodeCathedral);

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
	shader->SetUniform1i("iNumPointLights", 3); // Set number of lights
	scene.Render(camera, GetFrameAspect());
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

	if (glfwGetKey(GetWindow(), GLFW_KEY_1) == GLFW_PRESS)
		shader->FragmentSubRoutines(0);
	if (glfwGetKey(GetWindow(), GLFW_KEY_2) == GLFW_PRESS)
		shader->FragmentSubRoutines(1);
}
//=============================================================================