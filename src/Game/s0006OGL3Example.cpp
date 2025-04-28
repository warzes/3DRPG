#include "s0006OGL3Example.h"
//=============================================================================
namespace
{
	class BaseLight
	{
	public:
		Vector3f Color = Vector3f(1.0f, 1.0f, 1.0f);
		float AmbientIntensity = 0.0f;
	};

	class LightingTechnique : public utils::Technique
	{
	public:
		virtual bool Init();

		void SetWVP(const Matrix4f& WVP);
		void SetTextureUnit(unsigned int TextureUnit);
		void SetLight(const BaseLight& Light);
		void SetMaterial(const utils::Material& material);

	private:

		GLuint WVPLoc;
		GLuint samplerLoc;
		GLuint lightColorLoc;
		GLuint lightAmbientIntensityLoc;
		GLuint materialAmbientColorLoc;
	};

	bool LightingTechnique::Init()
	{
		if (!Technique::Init()) {
			return false;
		}

		if (!addShader(GL_VERTEX_SHADER, "shaders/0006lighting.vs")) {
			return false;
		}

		if (!addShader(GL_FRAGMENT_SHADER, "shaders/0006lighting.fs")) {
			return false;
		}

		if (!finalize()) {
			return false;
		}

		WVPLoc = getUniformLocation("gWVP");
		samplerLoc = getUniformLocation("gSampler");
		lightColorLoc = getUniformLocation("gLight.Color");
		lightAmbientIntensityLoc = getUniformLocation("gLight.AmbientIntensity");
		materialAmbientColorLoc = getUniformLocation("gMaterial.AmbientColor");

		if (lightAmbientIntensityLoc == 0xFFFFFFFF ||
			WVPLoc == 0xFFFFFFFF ||
			samplerLoc == 0xFFFFFFFF ||
			lightColorLoc == 0xFFFFFFFF ||
			materialAmbientColorLoc == 0xFFF)
		{
			return false;
		}

		return true;
	}
	void LightingTechnique::SetWVP(const Matrix4f& WVP)
	{
		glUniformMatrix4fv(WVPLoc, 1, GL_TRUE, (const GLfloat*)WVP.m);
	}
	void LightingTechnique::SetTextureUnit(unsigned int TextureUnit)
	{
		glUniform1i(samplerLoc, TextureUnit);
	}
	void LightingTechnique::SetLight(const BaseLight& Light)
	{
		glUniform3f(lightColorLoc, Light.Color.x, Light.Color.y, Light.Color.z);
		glUniform1f(lightAmbientIntensityLoc, Light.AmbientIntensity);
	}
	void LightingTechnique::SetMaterial(const utils::Material& material)
	{
		glUniform3f(materialAmbientColorLoc, material.ambientColor.r, material.ambientColor.g, material.ambientColor.b);
	}

	float screenAspect{ 1.0f };
	uint32_t    windowWidth{ 0 };
	uint32_t    windowHeight{ 0 };

	int cursorLastX;
	int cursorLastY;
	bool firstMouse = true;

	utils::Mesh* mesh;
	utils::CameraTemp* pGameCamera = NULL;
	PersProjInfo persProjInfo;
	LightingTechnique* pLightingTech = NULL;
	BaseLight baseLight;
}
//=============================================================================
s0006OGL3Example::~s0006OGL3Example()
{
	delete pLightingTech;
	delete pGameCamera;
	delete mesh;
}
//=============================================================================
void s0006OGL3Example::OnStart()
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

	baseLight.AmbientIntensity = 1.0f;

	Vector3f CameraPos(0.0f, 0.0f, -1.0f);
	Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
	Vector3f CameraUp(0.0f, 1.0f, 0.0f);

	pGameCamera = new utils::CameraTemp(windowWidth, windowHeight, CameraPos, CameraTarget, CameraUp);

	mesh = new utils::Mesh("Data/Mesh/spider.obj");

	pLightingTech = new LightingTechnique();
	if (!pLightingTech->Init())
	{
		return;
	}
	pLightingTech->Enable();
	pLightingTech->SetTextureUnit(COLOR_TEXTURE_UNIT_INDEX);

	
}
//=============================================================================
void s0006OGL3Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
	screenAspect = (float)width / (float)height;
	windowWidth = width;
	windowHeight = height;
}
//=============================================================================
void s0006OGL3Example::OnMouseMove(int xpos, int ypos)
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
void s0006OGL3Example::OnUpdate(float deltaTime)
{
	//if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
}
//=============================================================================
void s0006OGL3Example::OnRender()
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	pLightingTech->SetWVP(WVP);
	pLightingTech->SetLight(baseLight);
	pLightingTech->SetMaterial(mesh->GetMaterial());

	mesh->Render();
}
//=============================================================================
void s0006OGL3Example::OnImGuiDraw()
{

}
//=============================================================================