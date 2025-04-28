#include "s0007OGL3Example.h"
//=============================================================================
namespace
{
	class BaseLight
	{
	public:
		Vector3f Color = Vector3f(1.0f, 1.0f, 1.0f);
		float AmbientIntensity = 0.0f;
	};

	class DirectionalLight : public BaseLight
	{
	public:
		Vector3f WorldDirection = Vector3f(0.0f, 0.0f, 0.0f);
		float DiffuseIntensity = 0.0f;

		void CalcLocalDirection(const Matrix4f& World);

		const Vector3f& GetLocalDirection() const { return LocalDirection; }

	private:
		Vector3f LocalDirection = Vector3f(0.0f, 0.0f, 0.0f);
	};

	class LightingTechnique : public utils::Technique
	{
	public:
		virtual bool Init();

		void SetWVP(const Matrix4f& WVP);
		void SetTextureUnit(unsigned int TextureUnit);
		void SetDirectionalLight(const DirectionalLight& Light);
		void SetMaterial(const utils::Material& material);

	private:
		GLuint WVPLoc;
		GLuint samplerLoc;

		struct {
			GLuint AmbientColor;
			GLuint DiffuseColor;
		} materialLoc;

		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint Direction;
			GLuint DiffuseIntensity;
		} dirLightLoc;
	};

	void DirectionalLight::CalcLocalDirection(const Matrix4f& World)
	{
		Matrix3f World3f(World);  // Initialize using the top left corner

		// Inverse local-to-world transformation using transpose
		// (assuming uniform scaling)
		Matrix3f WorldToLocal = World3f.Transpose();

		LocalDirection = WorldToLocal * WorldDirection;

		LocalDirection = LocalDirection.Normalize();
	}

	bool LightingTechnique::Init()
	{
		if (!Technique::Init()) {
			return false;
		}

		if (!addShader(GL_VERTEX_SHADER, "shaders/0007lighting.vs")) {
			return false;
		}

		if (!addShader(GL_FRAGMENT_SHADER, "shaders/0007lighting.fs")) {
			return false;
		}

		if (!finalize()) {
			return false;
		}

		WVPLoc = getUniformLocation("gWVP");
		samplerLoc = getUniformLocation("gSampler");
		materialLoc.AmbientColor = getUniformLocation("gMaterial.AmbientColor");
		materialLoc.DiffuseColor = getUniformLocation("gMaterial.DiffuseColor");
		dirLightLoc.Color = getUniformLocation("gDirectionalLight.Color");
		dirLightLoc.AmbientIntensity = getUniformLocation("gDirectionalLight.AmbientIntensity");
		dirLightLoc.Direction = getUniformLocation("gDirectionalLight.Direction");
		dirLightLoc.DiffuseIntensity = getUniformLocation("gDirectionalLight.DiffuseIntensity");

		if (WVPLoc == 0xFFFFFFFF ||
			samplerLoc == 0xFFFFFFFF ||
			materialLoc.AmbientColor == 0xFFFFFFFF ||
			materialLoc.DiffuseColor == 0xFFFFFFFF ||
			dirLightLoc.Color == 0xFFFFFFFF ||
			dirLightLoc.DiffuseIntensity == 0xFFFFFFFF ||
			dirLightLoc.Direction == 0xFFFFFFFF ||
			dirLightLoc.AmbientIntensity == 0xFFFFFFFF)
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


	void LightingTechnique::SetDirectionalLight(const DirectionalLight& Light)
	{
		glUniform3f(dirLightLoc.Color, Light.Color.x, Light.Color.y, Light.Color.z);
		glUniform1f(dirLightLoc.AmbientIntensity, Light.AmbientIntensity);
		Vector3f LocalDirection = Light.GetLocalDirection();
		glUniform3f(dirLightLoc.Direction, LocalDirection.x, LocalDirection.y, LocalDirection.z);
		glUniform1f(dirLightLoc.DiffuseIntensity, Light.DiffuseIntensity);
	}


	void LightingTechnique::SetMaterial(const utils::Material& material)
	{
		glUniform3f(materialLoc.AmbientColor, material.ambientColor.r, material.ambientColor.g, material.ambientColor.b);
		glUniform3f(materialLoc.DiffuseColor, material.diffuseColor.r, material.diffuseColor.g, material.diffuseColor.b);
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
	DirectionalLight dirLight;
}
//=============================================================================
s0007OGL3Example::~s0007OGL3Example()
{
	delete pLightingTech;
	delete pGameCamera;
	delete mesh;
}
//=============================================================================
void s0007OGL3Example::OnStart()
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

	dirLight.AmbientIntensity = 0.1f;
	dirLight.DiffuseIntensity = 1.0f;
	dirLight.WorldDirection = Vector3f(1.0f, 0.0, 0.0);

	Vector3f CameraPos(0.0f, 0.0f, -3.0f);
	Vector3f CameraTarget(0.0f, 0.0f, 1.0f);
	Vector3f CameraUp(0.0f, 1.0f, 0.0f);

	pGameCamera = new utils::CameraTemp(windowWidth, windowHeight, CameraPos, CameraTarget, CameraUp);

	mesh = new utils::Mesh("Data/Mesh/box.obj");

	pLightingTech = new LightingTechnique();
	if (!pLightingTech->Init())
	{
		return;
	}
	pLightingTech->Enable();
	pLightingTech->SetTextureUnit(COLOR_TEXTURE_UNIT_INDEX);


}
//=============================================================================
void s0007OGL3Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
	screenAspect = (float)width / (float)height;
	windowWidth = width;
	windowHeight = height;
}
//=============================================================================
void s0007OGL3Example::OnMouseMove(int xpos, int ypos)
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
void s0007OGL3Example::OnUpdate(float deltaTime)
{
	//if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
}
//=============================================================================
void s0007OGL3Example::OnRender()
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pGameCamera->OnRender();

	float YRotationAngle = 0.1f;

	utils::WorldTrans& worldTransform = mesh->GetWorldTransform();

	worldTransform.SetScale(1.0f);
	worldTransform.SetPosition(0.0f, 0.0f, 2.0f);
	worldTransform.Rotate(0.0f, YRotationAngle, 0.0f);

	Matrix4f World = worldTransform.GetMatrix();
	dirLight.CalcLocalDirection(World);
	Matrix4f View = pGameCamera->GetMatrix();

	Matrix4f Projection;
	Projection.InitPersProjTransform(persProjInfo);

	Matrix4f WVP = Projection * View * World;
	pLightingTech->SetWVP(WVP);
	pLightingTech->SetDirectionalLight(dirLight);
	pLightingTech->SetMaterial(mesh->GetMaterial());

	mesh->Render();
}
//=============================================================================
void s0007OGL3Example::OnImGuiDraw()
{

}
//=============================================================================