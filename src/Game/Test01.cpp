#include "stdafx.h"
#include "Test01.h"
#include "Utility.h"
//=============================================================================
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
bool findSceneNode(aiNode* p_Node, const aiString& Name, const aiScene* p_Scene, const glm::mat4& m4Transform, glm::mat4& m4RetTransform)
{
	// Update current transform
	glm::mat4 m4CurrentTransform = glm::transpose(*(glm::mat4*)&p_Node->mTransformation) * m4Transform;
	if (strcmp(p_Node->mName.data, Name.data) == 0)
	{
		m4RetTransform = m4CurrentTransform;
		return true;
	}

	// Loop over each child node
	for (unsigned i = 0; i < p_Node->mNumChildren; i++) {
		bool bRet = findSceneNode(p_Node->mChildren[i], Name, p_Scene, m4CurrentTransform, m4RetTransform);
		if (bRet) {
			return true;
		}
	}
	return false;
}
//=============================================================================
void LoadLight(const std::string& path)
{
	// Load scene from file
	Assimp::Importer importer;
	const aiScene* aiscene = importer.ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_ImproveCacheLocality |
		aiProcess_SortByPType |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph
	);

	if (!aiscene || aiscene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiscene->mRootNode)
	{
		Error("Failed to open scene file: " + std::string(importer.GetErrorString()));
		return;
	}

	// Load in each light
	for (unsigned i = 0; i < aiscene->mNumLights; i++)
	{
		const aiLight* p_AILight = aiscene->mLights[i];
		glm::mat4 m4Ret(1.0f);
		findSceneNode(aiscene->mRootNode, p_AILight->mName, aiscene, m4Ret, m4Ret);
		if (p_AILight->mType == aiLightSource_POINT)
		{
			// Get point light
			glm::vec3 v3Position = glm::vec3(
				p_AILight->mPosition.x,
				p_AILight->mPosition.y,
				p_AILight->mPosition.z);
			v3Position = (glm::vec3)(m4Ret * glm::vec4(v3Position, 1.0f));
			glm::vec3 colour = glm::vec3(
				p_AILight->mColorDiffuse.r,
				p_AILight->mColorDiffuse.g,
				p_AILight->mColorDiffuse.b);
			// Divide linear and quadratic components by 2 to compensate for using a minimum attenuation of 1
			glm::vec3 falloff = glm::vec3(
				(p_AILight->mAttenuationConstant == 0.0f) ? 1.0f : p_AILight->mAttenuationConstant,
				p_AILight->mAttenuationLinear / 2.0f,
				p_AILight->mAttenuationQuadratic / 2.0f);

			scene.SetPointLight(i, true, v3Position, colour, falloff);
		}
	}
}
//=============================================================================
bool Test01::Init()
{
	rhi::Init();

	scene.Init();

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

	nodeCathedral.SetModel(modelCathedral);
	nodeCathedral.GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	//scene.AddNode(&nodeCathedral);

	nodeSphere.SetModel(modelSphere);
	nodeSphere.GetTransform().SetPosition(glm::vec3(-2.0f, 0.0f, -5.0f));
	scene.AddNode(&nodeSphere);

	nodeCube.SetModel(modelCube);
	nodeCube.GetTransform().SetPosition(glm::vec3(2.0f, 0.0f, -5.0f));
	scene.AddNode(&nodeCube);

	node.SetModel(model);
	node.GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	node.GetTransform().Rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	scene.AddNode(&node);

	LoadLight("data/Cathedral/TutorialCathedral.fbx");

	return true;
}
//=============================================================================
void Test01::Close()
{
	scene.Close();
	ClearDefaultGraphicsResource();
	rhi::Close();
}
//=============================================================================
void Test01::FixedUpdate(double deltaTime)
{
}
//=============================================================================
void Test01::Frame(double deltaTime)
{
	ProcessInput(camera, deltaTime, firstMouse, lastX, lastY);

	glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.Render(camera, GetFrameAspect());
}
//=============================================================================
void Test01::DrawImGui(double deltaTime)
{

}
//=============================================================================
void Test01::ProcessInput(Camera& camera, float deltaTime, bool& firstMouse, float& lastX, float& lastY)
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