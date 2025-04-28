#include "s0009OGL3Example.h"
//=============================================================================
namespace
{
	class BaseLight
	{
	public:
		Vector3f Color = Vector3f(1.0f, 1.0f, 1.0f);
		float AmbientIntensity = 0.0f;
		float DiffuseIntensity = 0.0f;
	};

	class DirectionalLight : public BaseLight
	{
	public:
		Vector3f WorldDirection = Vector3f(0.0f, 0.0f, 0.0f);

		void CalcLocalDirection(const Matrix4f& World);

		const Vector3f& GetLocalDirection() const { return LocalDirection; }

	private:
		Vector3f LocalDirection = Vector3f(0.0f, 0.0f, 0.0f);
	};

	struct LightAttenuation
	{
		float Constant = 1.0f;
		float Linear = 0.0f;
		float Exp = 0.0f;
	};

	class PointLight : public BaseLight
	{
	public:
		Vector3f WorldPosition = Vector3f(0.0f, 0.0f, 0.0f);
		LightAttenuation Attenuation;

		void CalcLocalPosition(const utils::WorldTrans& worldTransform);

		const Vector3f& GetLocalPosition() const { return LocalPosition; }

	private:
		Vector3f LocalPosition = Vector3f(0.0f, 0.0f, 0.0f);
	};

	class LightingTechnique : public utils::Technique
	{
	public:
		static const unsigned int MAX_POINT_LIGHTS = 2;

		virtual bool Init();

		void SetWVP(const Matrix4f& WVP);
		void SetTextureUnit(unsigned int TextureUnit);
		void SetSpecularExponentTextureUnit(unsigned int TextureUnit);
		void SetDirectionalLight(const DirectionalLight& Light);
		void SetPointLights(unsigned int NumLights, const PointLight* pLights);
		void SetCameraLocalPos(const Vector3f& CameraLocalPos);
		void SetMaterial(const utils::Material& material);

	private:
		GLuint WVPLoc;
		GLuint samplerLoc;
		GLuint samplerSpecularExponentLoc;
		GLuint CameraLocalPosLoc;
		GLuint NumPointLightsLocation;

		struct {
			GLuint AmbientColor;
			GLuint DiffuseColor;
			GLuint SpecularColor;
		} materialLoc;

		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint Direction;
			GLuint DiffuseIntensity;
		} dirLightLoc;

		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint Position;
			GLuint DiffuseIntensity;

			struct
			{
				GLuint Constant;
				GLuint Linear;
				GLuint Exp;
			} Atten;
		} PointLightsLocation[MAX_POINT_LIGHTS];
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

	void PointLight::CalcLocalPosition(const utils::WorldTrans& worldTransform)
	{
		LocalPosition = worldTransform.WorldPosToLocalPos(WorldPosition);
	}

	bool LightingTechnique::Init()
	{
		if (!Technique::Init()) {
			return false;
		}

		if (!addShader(GL_VERTEX_SHADER, "shaders/0009lighting.vs")) {
			return false;
		}

		if (!addShader(GL_FRAGMENT_SHADER, "shaders/0009lighting.fs")) {
			return false;
		}

		if (!finalize()) {
			return false;
		}

		WVPLoc = getUniformLocation("gWVP");
		samplerLoc = getUniformLocation("gSampler");
		samplerSpecularExponentLoc = getUniformLocation("gSamplerSpecularExponent");
		materialLoc.AmbientColor = getUniformLocation("gMaterial.AmbientColor");
		materialLoc.DiffuseColor = getUniformLocation("gMaterial.DiffuseColor");
		materialLoc.SpecularColor = getUniformLocation("gMaterial.SpecularColor");
		dirLightLoc.Color = getUniformLocation("gDirectionalLight.Base.Color");
		dirLightLoc.AmbientIntensity = getUniformLocation("gDirectionalLight.Base.AmbientIntensity");
		dirLightLoc.Direction = getUniformLocation("gDirectionalLight.Direction");
		dirLightLoc.DiffuseIntensity = getUniformLocation("gDirectionalLight.Base.DiffuseIntensity");
		CameraLocalPosLoc = getUniformLocation("gCameraLocalPos");
		NumPointLightsLocation = getUniformLocation("gNumPointLights");

		for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(PointLightsLocation); i++) {
			char Name[128];
			memset(Name, 0, sizeof(Name));
			_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Base.Color", i);
			PointLightsLocation[i].Color = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Base.AmbientIntensity", i);
			PointLightsLocation[i].AmbientIntensity = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gPointLights[%d].LocalPos", i);
			PointLightsLocation[i].Position = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Base.DiffuseIntensity", i);
			PointLightsLocation[i].DiffuseIntensity = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Atten.Constant", i);
			PointLightsLocation[i].Atten.Constant = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Atten.Linear", i);
			PointLightsLocation[i].Atten.Linear = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gPointLights[%d].Atten.Exp", i);
			PointLightsLocation[i].Atten.Exp = getUniformLocation(Name);

			if (PointLightsLocation[i].Color == INVALID_UNIFORM_LOCATION ||
				PointLightsLocation[i].AmbientIntensity == INVALID_UNIFORM_LOCATION ||
				PointLightsLocation[i].Position == INVALID_UNIFORM_LOCATION ||
				PointLightsLocation[i].DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
				PointLightsLocation[i].Atten.Constant == INVALID_UNIFORM_LOCATION ||
				PointLightsLocation[i].Atten.Linear == INVALID_UNIFORM_LOCATION ||
				PointLightsLocation[i].Atten.Exp == INVALID_UNIFORM_LOCATION) {
				return false;
			}
		}

		if (WVPLoc == 0xFFFFFFFF ||
			samplerLoc == 0xFFFFFFFF ||
			samplerSpecularExponentLoc == 0xFFFFFFFF ||
			materialLoc.AmbientColor == 0xFFFFFFFF ||
			materialLoc.DiffuseColor == 0xFFFFFFFF ||
			materialLoc.SpecularColor == 0xFFFFFFFF ||
			CameraLocalPosLoc == 0xFFFFFFFF ||
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

	void LightingTechnique::SetSpecularExponentTextureUnit(unsigned int TextureUnit)
	{
		glUniform1i(samplerSpecularExponentLoc, TextureUnit);
	}


	void LightingTechnique::SetDirectionalLight(const DirectionalLight& Light)
	{
		glUniform3f(dirLightLoc.Color, Light.Color.x, Light.Color.y, Light.Color.z);
		glUniform1f(dirLightLoc.AmbientIntensity, Light.AmbientIntensity);
		Vector3f LocalDirection = Light.GetLocalDirection();
		glUniform3f(dirLightLoc.Direction, LocalDirection.x, LocalDirection.y, LocalDirection.z);
		glUniform1f(dirLightLoc.DiffuseIntensity, Light.DiffuseIntensity);
	}


	void LightingTechnique::SetCameraLocalPos(const Vector3f& CameraLocalPos)
	{
		glUniform3f(CameraLocalPosLoc, CameraLocalPos.x, CameraLocalPos.y, CameraLocalPos.z);
	}


	void LightingTechnique::SetMaterial(const utils::Material& material)
	{
		glUniform3f(materialLoc.AmbientColor, material.ambientColor.r, material.ambientColor.g, material.ambientColor.b);
		glUniform3f(materialLoc.DiffuseColor, material.diffuseColor.r, material.diffuseColor.g, material.diffuseColor.b);
		glUniform3f(materialLoc.SpecularColor, material.specularColor.r, material.specularColor.g, material.specularColor.b);
	}

	void LightingTechnique::SetPointLights(unsigned int NumLights, const PointLight* pLights)
	{
		glUniform1i(NumPointLightsLocation, NumLights);

		for (unsigned int i = 0; i < NumLights; i++) {
			glUniform3f(PointLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
			glUniform1f(PointLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
			glUniform1f(PointLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
			const Vector3f& LocalPos = pLights[i].GetLocalPosition();
			//LocalPos.Print();printf("\n");
			glUniform3f(PointLightsLocation[i].Position, LocalPos.x, LocalPos.y, LocalPos.z);
			glUniform1f(PointLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
			glUniform1f(PointLightsLocation[i].Atten.Linear, pLights[i].Attenuation.Linear);
			glUniform1f(PointLightsLocation[i].Atten.Exp, pLights[i].Attenuation.Exp);
		}
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
	PointLight pointLights[LightingTechnique::MAX_POINT_LIGHTS];
	float counter = 0;
}
//=============================================================================
s0009OGL3Example::~s0009OGL3Example()
{
	delete pLightingTech;
	delete pGameCamera;
	delete mesh;
}
//=============================================================================
void s0009OGL3Example::OnStart()
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

	pointLights[0].DiffuseIntensity = 1.0f;
	pointLights[0].Color = Vector3f(1.0f, 1.0f, 1.0f);
	pointLights[0].Attenuation.Linear = 0.2f;
	pointLights[0].Attenuation.Exp = 0.0f;

	pointLights[1].DiffuseIntensity = 1.0f;
	pointLights[1].Color = Vector3f(1.0f, 1.0f, 1.0f);
	pointLights[1].Attenuation.Linear = 0.0f;
	pointLights[1].Attenuation.Exp = 0.2f;

	Vector3f CameraPos(0.0f, 5.0f, -8.0f);
	Vector3f CameraTarget(0.0f, -0.5f, 1.0f);
	Vector3f CameraUp(0.0f, 1.0f, 0.0f);

	pGameCamera = new utils::CameraTemp(windowWidth, windowHeight, CameraPos, CameraTarget, CameraUp);

	mesh = new utils::Mesh("Data/Mesh/box_terrain.obj");

	pLightingTech = new LightingTechnique();
	if (!pLightingTech->Init())
	{
		return;
	}
	pLightingTech->Enable();
	pLightingTech->SetTextureUnit(COLOR_TEXTURE_UNIT_INDEX);
	pLightingTech->SetSpecularExponentTextureUnit(SPECULAR_EXPONENT_UNIT_INDEX);
}
//=============================================================================
void s0009OGL3Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
	screenAspect = (float)width / (float)height;
	windowWidth = width;
	windowHeight = height;
}
//=============================================================================
void s0009OGL3Example::OnMouseMove(int xpos, int ypos)
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
void s0009OGL3Example::OnUpdate(float deltaTime)
{
#define ATTEN_STEP 0.01f

	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		pointLights[0].Attenuation.Linear += ATTEN_STEP * deltaTime;
		pointLights[1].Attenuation.Linear += ATTEN_STEP * deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		pointLights[0].Attenuation.Linear -= ATTEN_STEP * deltaTime;
		pointLights[1].Attenuation.Linear -= ATTEN_STEP * deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		pointLights[0].Attenuation.Exp += ATTEN_STEP * deltaTime;
		pointLights[1].Attenuation.Exp += ATTEN_STEP * deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_X) == GLFW_PRESS)
	{
		pointLights[0].Attenuation.Exp -= ATTEN_STEP * deltaTime;
		pointLights[1].Attenuation.Exp -= ATTEN_STEP * deltaTime;
	}
}
//=============================================================================
void s0009OGL3Example::OnRender()
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pGameCamera->OnRender();

	float YRotationAngle = 0.1f;

	utils::WorldTrans& worldTransform = mesh->GetWorldTransform();

	worldTransform.SetRotation(0.0f, 0.0f, 0.0f);
	worldTransform.SetPosition(0.0f, 0.0f, 10.0f);

	worldTransform.SetScale(1.0f);
	worldTransform.SetPosition(0.0f, 0.0f, 2.0f);
	worldTransform.Rotate(0.0f, YRotationAngle, 0.0f);

	Matrix4f World = worldTransform.GetMatrix();
	Matrix4f View = pGameCamera->GetMatrix();
	Matrix4f Projection;
	Projection.InitPersProjTransform(persProjInfo);
	Matrix4f WVP = Projection * View * World;
	pLightingTech->SetWVP(WVP);

	counter += 0.01f;
	pointLights[0].WorldPosition.x = -8.0f;
	pointLights[0].WorldPosition.y = sinf(counter) * 4 + 4;
	pointLights[0].WorldPosition.z = 0.0f;
	pointLights[0].CalcLocalPosition(worldTransform);

	pointLights[1].WorldPosition.x = 8.0f;
	pointLights[1].WorldPosition.y = sinf(counter) * 4 + 4;
	pointLights[1].WorldPosition.z = 0.0f;
	pointLights[1].CalcLocalPosition(worldTransform);

	pLightingTech->SetPointLights(2, pointLights);

	pLightingTech->SetMaterial(mesh->GetMaterial());

	Vector3f CameraLocalPos3f = worldTransform.WorldPosToLocalPos(pGameCamera->GetPos());
	pLightingTech->SetCameraLocalPos(CameraLocalPos3f);

	mesh->Render();
}
//=============================================================================
void s0009OGL3Example::OnImGuiDraw()
{

}
//=============================================================================