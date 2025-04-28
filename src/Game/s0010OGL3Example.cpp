#include "s0010OGL3Example.h"
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

		void CalcLocalDirection(const utils::WorldTrans& worldTransform);

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

	class SpotLight : public PointLight
	{
	public:
		Vector3f WorldDirection = Vector3f(0.0f, 0.0f, 0.0f);
		float Cutoff = 0.0f;

		void CalcLocalDirectionAndPosition(const utils::WorldTrans& worldTransform);

		const Vector3f& GetLocalDirection() const { return LocalDirection; }

	private:
		Vector3f LocalDirection = Vector3f(0.0f, 0.0f, 0.0f);
	};

	class LightingTechnique : public utils::Technique
	{
	public:
		static const unsigned int MAX_POINT_LIGHTS = 2;
		static const unsigned int MAX_SPOT_LIGHTS = 2;

		virtual bool Init();

		void SetWVP(const Matrix4f& WVP);
		void SetTextureUnit(unsigned int TextureUnit);
		void SetSpecularExponentTextureUnit(unsigned int TextureUnit);
		void SetDirectionalLight(const DirectionalLight& Light);
		void SetPointLights(unsigned int NumLights, const PointLight* pLights);
		void SetSpotLights(unsigned int NumLights, const SpotLight* pLights);
		void SetCameraLocalPos(const Vector3f& CameraLocalPos);
		void SetMaterial(const utils::Material& material);

	private:
		GLuint WVPLoc;
		GLuint samplerLoc;
		GLuint samplerSpecularExponentLoc;
		GLuint CameraLocalPosLoc;
		GLuint NumPointLightsLocation;
		GLuint NumSpotLightsLocation;

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

		struct {
			GLuint Color;
			GLuint AmbientIntensity;
			GLuint DiffuseIntensity;
			GLuint Position;
			GLuint Direction;
			GLuint Cutoff;
			struct {
				GLuint Constant;
				GLuint Linear;
				GLuint Exp;
			} Atten;
		} SpotLightsLocation[MAX_SPOT_LIGHTS];
	};

	void DirectionalLight::CalcLocalDirection(const utils::WorldTrans& worldTransform)
	{
		LocalDirection = worldTransform.WorldDirToLocalDir(WorldDirection);
	}

	void PointLight::CalcLocalPosition(const utils::WorldTrans& worldTransform)
	{
		LocalPosition = worldTransform.WorldPosToLocalPos(WorldPosition);
	}


	void SpotLight::CalcLocalDirectionAndPosition(const utils::WorldTrans& worldTransform)
	{
		CalcLocalPosition(worldTransform);

		LocalDirection = worldTransform.WorldDirToLocalDir(WorldDirection);
	}

	bool LightingTechnique::Init()
	{
		if (!Technique::Init()) {
			return false;
		}

		if (!addShader(GL_VERTEX_SHADER, "shaders/0010lighting.vs")) {
			return false;
		}

		if (!addShader(GL_FRAGMENT_SHADER, "shaders/0010lighting.fs")) {
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
		NumSpotLightsLocation = getUniformLocation("gNumSpotLights");

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
			dirLightLoc.AmbientIntensity == 0xFFFFFFFF ||
			NumPointLightsLocation == INVALID_UNIFORM_LOCATION ||
			NumSpotLightsLocation == INVALID_UNIFORM_LOCATION)
		{
#ifdef FAIL_ON_MISSING_LOC
			return false;
#endif
		}


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
#ifdef FAIL_ON_MISSING_LOC
				return false;
#endif
			}
		}

		for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(SpotLightsLocation); i++) {
			char Name[128];
			memset(Name, 0, sizeof(Name));
			_snprintf_s(Name, sizeof(Name), "gSpotLights[%d].Base.Base.Color", i);
			SpotLightsLocation[i].Color = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gSpotLights[%d].Base.Base.AmbientIntensity", i);
			SpotLightsLocation[i].AmbientIntensity = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gSpotLights[%d].Base.LocalPos", i);
			SpotLightsLocation[i].Position = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gSpotLights[%d].Direction", i);
			SpotLightsLocation[i].Direction = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gSpotLights[%d].Cutoff", i);
			SpotLightsLocation[i].Cutoff = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gSpotLights[%d].Base.Base.DiffuseIntensity", i);
			SpotLightsLocation[i].DiffuseIntensity = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Constant", i);
			SpotLightsLocation[i].Atten.Constant = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Linear", i);
			SpotLightsLocation[i].Atten.Linear = getUniformLocation(Name);

			_snprintf_s(Name, sizeof(Name), "gSpotLights[%d].Base.Atten.Exp", i);
			SpotLightsLocation[i].Atten.Exp = getUniformLocation(Name);

			if (SpotLightsLocation[i].Color == INVALID_UNIFORM_LOCATION ||
				SpotLightsLocation[i].AmbientIntensity == INVALID_UNIFORM_LOCATION ||
				SpotLightsLocation[i].Position == INVALID_UNIFORM_LOCATION ||
				SpotLightsLocation[i].Direction == INVALID_UNIFORM_LOCATION ||
				SpotLightsLocation[i].Cutoff == INVALID_UNIFORM_LOCATION ||
				SpotLightsLocation[i].DiffuseIntensity == INVALID_UNIFORM_LOCATION ||
				SpotLightsLocation[i].Atten.Constant == INVALID_UNIFORM_LOCATION ||
				SpotLightsLocation[i].Atten.Linear == INVALID_UNIFORM_LOCATION ||
				SpotLightsLocation[i].Atten.Exp == INVALID_UNIFORM_LOCATION) {
#ifdef FAIL_ON_MISSING_LOC
				return false;
#endif
			}
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

	void LightingTechnique::SetSpotLights(unsigned int NumLights, const SpotLight* pLights)
	{
		glUniform1i(NumSpotLightsLocation, NumLights);

		for (unsigned int i = 0; i < NumLights; i++) {
			glUniform3f(SpotLightsLocation[i].Color, pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z);
			glUniform1f(SpotLightsLocation[i].AmbientIntensity, pLights[i].AmbientIntensity);
			glUniform1f(SpotLightsLocation[i].DiffuseIntensity, pLights[i].DiffuseIntensity);
			const Vector3f& LocalPos = pLights[i].GetLocalPosition();
			glUniform3f(SpotLightsLocation[i].Position, LocalPos.x, LocalPos.y, LocalPos.z);
			Vector3f Direction = pLights[i].GetLocalDirection();
			Direction.Normalize();
			glUniform3f(SpotLightsLocation[i].Direction, Direction.x, Direction.y, Direction.z);
			glUniform1f(SpotLightsLocation[i].Cutoff, cosf(ToRadian(pLights[i].Cutoff)));
			glUniform1f(SpotLightsLocation[i].Atten.Constant, pLights[i].Attenuation.Constant);
			glUniform1f(SpotLightsLocation[i].Atten.Linear, pLights[i].Attenuation.Linear);
			glUniform1f(SpotLightsLocation[i].Atten.Exp, pLights[i].Attenuation.Exp);
		}
	}

	float screenAspect{ 1.0f };
	uint32_t    windowWidth{ 0 };
	uint32_t    windowHeight{ 0 };

	int cursorLastX;
	int cursorLastY;
	bool firstMouse = true;

	utils::Mesh* pBox = NULL;
	utils::Mesh* pMesh1 = NULL;
	utils::Mesh* pMesh2 = NULL;
	utils::CameraTemp* pGameCamera = NULL;
	PersProjInfo persProjInfo;
	LightingTechnique* pLightingTech = NULL;
	PointLight pointLights[LightingTechnique::MAX_POINT_LIGHTS];
	SpotLight spotLights[LightingTechnique::MAX_SPOT_LIGHTS];
	float counter = 0;

	bool Start = false;
}
//=============================================================================
s0010OGL3Example::~s0010OGL3Example()
{
	delete pLightingTech;
	delete pGameCamera;
	delete pBox;
	delete pMesh1;
	delete pMesh2;
}
//=============================================================================
void s0010OGL3Example::OnStart()
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

	pointLights[0].DiffuseIntensity = 0.5f;
	pointLights[0].Color = Vector3f(1.0f, 1.0f, 0.0f);
	pointLights[0].Attenuation.Linear = 0.2f;
	pointLights[0].Attenuation.Exp = 0.0f;

	pointLights[1].DiffuseIntensity = 0.0f;
	pointLights[1].Color = Vector3f(0.0f, 1.0f, 1.0f);
	pointLights[1].Attenuation.Linear = 0.0f;
	pointLights[1].Attenuation.Exp = 0.2f;

	spotLights[0].DiffuseIntensity = 1.0f;
	spotLights[0].Color = Vector3f(1.0f, 0.0f, 0.0f);
	spotLights[0].Attenuation.Linear = 0.01f;
	spotLights[0].Cutoff = 20.0f;

	spotLights[1].DiffuseIntensity = 1.0f;
	spotLights[1].Color = Vector3f(1.0f, 1.0f, 1.0f);
	spotLights[1].Attenuation.Linear = 0.01f;
	spotLights[1].Cutoff = 30.0f;

	Vector3f CameraPos(0.0f, 5.0f, -8.0f);
	Vector3f CameraTarget(0.0f, -0.5f, 1.0f);
	Vector3f CameraUp(0.0f, 1.0f, 0.0f);

	pGameCamera = new utils::CameraTemp(windowWidth, windowHeight, CameraPos, CameraTarget, CameraUp);

	pBox = new utils::Mesh("Data/Mesh/box_terrain.obj");
	pMesh1 = new utils::Mesh("Data/Mesh/Zombie.obj");
	pMesh2 = new utils::Mesh("Data/Mesh/Zombie.obj"); // заменить модель

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
void s0010OGL3Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
	screenAspect = (float)width / (float)height;
	windowWidth = width;
	windowHeight = height;
}
//=============================================================================
void s0010OGL3Example::OnMouseMove(int xpos, int ypos)
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
void s0010OGL3Example::OnUpdate(float deltaTime)
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
void s0010OGL3Example::OnRender()
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pGameCamera->OnRender();

	float YRotationAngle = 0.1f;
	counter += 0.01f;

	utils::WorldTrans& worldTransform = pBox->GetWorldTransform();

	worldTransform.SetRotation(0.0f, 0.0f, 0.0f);
	worldTransform.SetPosition(0.0f, 0.0f, 10.0f);

	Matrix4f World = worldTransform.GetMatrix();
	Matrix4f View = pGameCamera->GetMatrix();
	Matrix4f Projection;
	Projection.InitPersProjTransform(persProjInfo);
	Matrix4f WVP = Projection * View * World;
	pLightingTech->SetWVP(WVP);

	pointLights[0].WorldPosition.x = -10.0f;
	pointLights[0].WorldPosition.y = 2;
	pointLights[0].WorldPosition.z = 0.0f;
	pointLights[0].CalcLocalPosition(worldTransform);

	pointLights[1].WorldPosition.x = 10.0f;
	pointLights[1].WorldPosition.y = sinf(counter) * 4 + 4;
	pointLights[1].WorldPosition.z = 0.0f;
	pointLights[1].CalcLocalPosition(worldTransform);

	pLightingTech->SetPointLights(2, pointLights);

	spotLights[0].WorldPosition = pGameCamera->GetPos();
	spotLights[0].WorldDirection = pGameCamera->GetTarget();
	spotLights[0].CalcLocalDirectionAndPosition(worldTransform);

	spotLights[1].WorldPosition = Vector3f(0.0f, 1.0f, 0.0f);
	spotLights[1].WorldDirection = Vector3f(0.0f, -1.0f, 0.0f);
	spotLights[1].CalcLocalDirectionAndPosition(worldTransform);

	pLightingTech->SetSpotLights(2, spotLights);

	pLightingTech->SetMaterial(pBox->GetMaterial());

	Vector3f CameraLocalPos3f = worldTransform.WorldPosToLocalPos(pGameCamera->GetPos());
	pLightingTech->SetCameraLocalPos(CameraLocalPos3f);

	pBox->Render();

	utils::WorldTrans& meshWorldTransform = pMesh1->GetWorldTransform();
	if (Start) {
		static float counter2 = 0.0f;
		counter2 += 0.001f;
		meshWorldTransform.SetPosition(0.0f, -4.0f + abs(sinf(counter2) * 4), 0.0f);
	}
	else {
		meshWorldTransform.SetPosition(0.0f, -4.0f, 0.0f);
	}

	World = meshWorldTransform.GetMatrix();
	WVP = Projection * View * World;
	pLightingTech->SetWVP(WVP);

	pointLights[0].CalcLocalPosition(meshWorldTransform);
	pointLights[1].CalcLocalPosition(meshWorldTransform);
	pLightingTech->SetPointLights(2, pointLights);

	spotLights[0].CalcLocalDirectionAndPosition(meshWorldTransform);
	spotLights[1].CalcLocalDirectionAndPosition(meshWorldTransform);
	pLightingTech->SetSpotLights(2, spotLights);

	pLightingTech->SetMaterial(pMesh1->GetMaterial());

	CameraLocalPos3f = meshWorldTransform.WorldPosToLocalPos(pGameCamera->GetPos());
	pLightingTech->SetCameraLocalPos(CameraLocalPos3f);

	pMesh1->Render();

	utils::WorldTrans& meshWorldTransform2 = pMesh2->GetWorldTransform();
	//  meshWorldTransform2.SetRotation(0.0f, -45.0f, 0.0f);
	meshWorldTransform2.SetPosition(0.0f, 1.0f, 1.0f);
	World = meshWorldTransform2.GetMatrix();
	WVP = Projection * View * World;
	pLightingTech->SetWVP(WVP);

	pointLights[0].CalcLocalPosition(meshWorldTransform2);
	pointLights[1].CalcLocalPosition(meshWorldTransform2);
	pLightingTech->SetPointLights(2, pointLights);

	spotLights[0].CalcLocalDirectionAndPosition(meshWorldTransform2);
	spotLights[1].CalcLocalDirectionAndPosition(meshWorldTransform2);
	pLightingTech->SetSpotLights(2, spotLights);

	pLightingTech->SetMaterial(pMesh2->GetMaterial());

	CameraLocalPos3f = meshWorldTransform2.WorldPosToLocalPos(pGameCamera->GetPos());
	pLightingTech->SetCameraLocalPos(CameraLocalPos3f);

	pMesh2->Render();
}
//=============================================================================
void s0010OGL3Example::OnImGuiDraw()
{

}
//=============================================================================