#include "stdafx.h"
#include "GLScene.h"
#include "Test.h"
#include "CoreApp.h"
#include "Render.h"
#include "Graphics.h"

int GetFrameWidth();
int GetFrameHeight();

//Shaders.cpp
extern bool GL_LoadShader(GLuint& uiShader, GLenum ShaderType, const GLchar* p_cShader);
extern bool GL_LoadShaders(GLuint& uiShader, GLuint uiVertexShader, GLuint uiFragmentShader, GLuint uiGeometryShader = -1, GLuint uiTessControlShader = -1, GLuint uiTessEvalShader = -1);
extern bool GL_LoadShaderFile(GLuint& uiShader, GLenum ShaderType, const char* p_cFileName, int iFileID);
//Textures.cpp
extern bool GL_LoadTextureKTX(GLuint uiTexture, const char* p_cTextureFile);
//Reflection.cpp
extern bool GL_InitReflection();
extern void GL_QuitReflection();
extern void GL_RenderPlanarReflection(ReflectObjectData* p_RObject, ObjectData* p_Object, const vec3& v3Direction, const vec3& v3Up,
	const vec3& v3Position, float fFOV, float fAspect, const vec2& v2NearFar);
extern void GL_RenderEnvironmentReflection(ReflectObjectData* p_RObject, ObjectData* p_Object);
//Shadow.cpp
extern bool GL_InitShadow();
extern void GL_QuitShadow();
extern void GL_RenderShadows();
//Deferred.cpp
extern bool GL_InitDeferred();
extern void GL_QuitDeferred();
extern void GL_RenderDeferred(ObjectData* p_Object = NULL, GLuint uiAccumBuffer = 0, GLenum uiTextureTarget = GL_TEXTURE_2D);
//PostProc.cpp
extern bool GL_InitPostProcess();
extern void GL_QuitPostProcess();
extern void GL_RenderPostProcess();

// Declare OpenGL variables
GLuint g_uiMainProgram;
GLuint g_uiShadowProgram;
GLuint g_uiShadowTransProgram;
GLuint g_uiDeferredProgram2;
GLuint g_uiPostProcProgram;
GLuint g_uiPostProcInitProgram;
GLuint g_uiGaussProgram;
GLuint g_uiSSAOProgram;
GLuint g_uiSSAOProgram2;
GLuint g_uiSpotSSVLProgram;
GLuint g_uiSpotSSVLProgram2;
SceneData g_SceneData = {}; // Init to 0
GLuint g_uiSkyBox;

struct CameraData
{
	aligned_mat4 m_m4ViewProjection;
	aligned_vec3 m_v3Position;
	aligned_mat4 m_m4InvViewProjection;
};

bool GL_Init()
{
	// Set up initial GL attributes
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set the cleared back buffer to black
	glCullFace(GL_BACK);                  // Set back-face culling
	glEnable(GL_CULL_FACE);               // Enable use of back/front face culling
	glEnable(GL_DEPTH_TEST);              // Enable use of depth testing
	glDisable(GL_STENCIL_TEST);           // Disable stencil test for speed
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // Enable texturing filtering across cube map faces
	//glEnable(GL_FRAMEBUFFER_SRGB);        // Enable sRGB conversion when outputting to sRGB buffer

	// Load in scene from file
	if (!GL_LoadScene("data/Cathedral/TutorialCathedral.fbx", g_SceneData)) {
		return false;
	}

	// Create vertex shader
	GLuint uiVertexShader;
	if (!GL_LoadShaderFile(uiVertexShader, GL_VERTEX_SHADER, "data/shaders/MainPNUTVert.glsl", 100))
		return false;

	// Create deferred fragment shader
	GLuint uiFragmentShader;
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/Deferred1stFrag.glsl", 200))
		return false;

	// Create tessellation shader
	GLuint uiTessControl;
	GLuint uiTessEval;
	if (!GL_LoadShaderFile(uiTessControl, GL_TESS_CONTROL_SHADER, "data/shaders/PNTriangleTessControl.glsl", 1100))
		return false;
	if (!GL_LoadShaderFile(uiTessEval, GL_TESS_EVALUATION_SHADER, "data/shaders/PNTriangleTessEval.glsl", 1200))
		return false;

	// Create program
	if (!GL_LoadShaders(g_uiMainProgram, uiVertexShader, uiFragmentShader, -1, uiTessControl, uiTessEval))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiVertexShader);
	glDeleteShader(uiFragmentShader);
	glDeleteShader(uiTessControl);
	glDeleteShader(uiTessEval);

	// Load in second deferred pass shader
	if (!GL_LoadShaderFile(uiVertexShader, GL_VERTEX_SHADER, "data/shaders/QuadVert.glsl", 300))
		return false;
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/Deferred2ndFrag.glsl", 400))
		return false;
	if (!GL_LoadShaders(g_uiDeferredProgram2, uiVertexShader, uiFragmentShader))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiFragmentShader);

	// Load in post processing pass shader
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/PostProcessFrag.glsl", 1300))
		return false;
	if (!GL_LoadShaders(g_uiPostProcProgram, uiVertexShader, uiFragmentShader))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiFragmentShader);

	// Load in post processing pass shader
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/PostProcessInitFrag.glsl", 1400))
		return false;
	if (!GL_LoadShaders(g_uiPostProcInitProgram, uiVertexShader, uiFragmentShader))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiFragmentShader);

	// Load in Gaussian blur shader
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/GaussBlurFrag.glsl", 1500))
		return false;
	if (!GL_LoadShaders(g_uiGaussProgram, uiVertexShader, uiFragmentShader))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiFragmentShader);

	// Load in ambient occlusion shader
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/PostProcessAO1stFrag.glsl", 1600))
		return false;
	if (!GL_LoadShaders(g_uiSSAOProgram, uiVertexShader, uiFragmentShader))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiFragmentShader);

	// Load in ambient occlusion shader
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/PostProcessAO2ndFrag.glsl", 1700))
		return false;
	if (!GL_LoadShaders(g_uiSSAOProgram2, uiVertexShader, uiFragmentShader))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiFragmentShader);

	// Load in ambient occlusion shader
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/PostProcessVolSpots1stFrag.glsl", 1800))
		return false;
	if (!GL_LoadShaders(g_uiSpotSSVLProgram, uiVertexShader, uiFragmentShader))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiFragmentShader);

	// Load in ambient occlusion shader
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/PostProcessVolSpots2ndFrag.glsl", 1900))
		return false;
	if (!GL_LoadShaders(g_uiSpotSSVLProgram2, uiVertexShader, uiFragmentShader))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiVertexShader);
	glDeleteShader(uiFragmentShader);

	// Load in shadow map shader
	GLuint uiGeometryShader;
	if (!GL_LoadShaderFile(uiVertexShader, GL_VERTEX_SHADER, "data/shaders/ShadowVert.glsl", 500))
		return false;
	if (!GL_LoadShaderFile(uiGeometryShader, GL_GEOMETRY_SHADER, "data/shaders/ShadowGeom.glsl", 600))
		return false;
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/ShadowFrag.glsl", 700))
		return false;
	if (!GL_LoadShaders(g_uiShadowProgram, uiVertexShader, uiFragmentShader, uiGeometryShader))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiVertexShader);
	glDeleteShader(uiGeometryShader);
	glDeleteShader(uiFragmentShader);

	// Load in shadow map transparency shader
	if (!GL_LoadShaderFile(uiVertexShader, GL_VERTEX_SHADER, "data/shaders/ShadowTransVert.glsl", 800))
		return false;
	if (!GL_LoadShaderFile(uiGeometryShader, GL_GEOMETRY_SHADER, "data/shaders/ShadowTransGeom.glsl", 900))
		return false;
	if (!GL_LoadShaderFile(uiFragmentShader, GL_FRAGMENT_SHADER, "data/shaders/ShadowTransFrag.glsl", 1000))
		return false;
	if (!GL_LoadShaders(g_uiShadowTransProgram, uiVertexShader, uiFragmentShader, uiGeometryShader))
		return false;

	// Clean up unneeded shaders
	glDeleteShader(uiVertexShader);
	glDeleteShader(uiGeometryShader);
	glDeleteShader(uiFragmentShader);

	// Bind light UBO
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, g_SceneData.m_uiPointLightUBO);
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, g_SceneData.m_uiSpotLightUBO);

	// Load SkyBox
	glGenTextures(1, &g_uiSkyBox);
	GL_LoadTextureKTX(g_uiSkyBox, "data/Cathedral/textures/SkyBox.ktx");

	// Bind and set up SkyBox texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, g_uiSkyBox);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, 4);

	// Setup tessellation parameters
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glProgramUniform2f(g_uiMainProgram, 4, (float)GetFrameWidth(), (float)GetFrameHeight());

	// Initialise deferred rendering
	GL_InitDeferred();

	// Initialise post-process
	GL_InitPostProcess();

	// Initialise shadows
	GL_InitShadow();

	// Initialise reflections
	GL_InitReflection();

	return true;
}

void GL_Quit()
{
	// Release the shader programs
	glDeleteProgram(g_uiMainProgram);
	glDeleteProgram(g_uiShadowProgram);
	glDeleteProgram(g_uiShadowTransProgram);
	glDeleteProgram(g_uiDeferredProgram2);
	glDeleteProgram(g_uiPostProcInitProgram);
	glDeleteProgram(g_uiPostProcProgram);
	glDeleteProgram(g_uiGaussProgram);
	glDeleteProgram(g_uiSSAOProgram);
	glDeleteProgram(g_uiSSAOProgram2);
	glDeleteProgram(g_uiSpotSSVLProgram);
	glDeleteProgram(g_uiSpotSSVLProgram2);

	// Release deferred data
	GL_QuitDeferred();

	// Release post-process data
	GL_QuitPostProcess();

	// Release shadow data
	GL_QuitShadow();

	// Release reflection data
	GL_QuitReflection();

	// Release Scene data
	GL_UnloadScene(g_SceneData);
}

void GL_RenderObjects(ObjectData* p_SkipObject = NULL)
{
	// Clear the render output and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Initialise sub routine selectors
	const GLuint uiEmissiveSubs[] = { 0, 1 };
	const GLuint uiRefractSubs[] = { 2, 3 };
	const GLuint uiReflectSubs[] = { 4, 5, 6 };

	// Loop through each object
	for (unsigned i = 0; i < g_SceneData.m_uiNumObjects; i++) {
		const ObjectData* p_Object = &g_SceneData.mp_Objects[i];

		// Check if this object should be rendered
		if (p_Object == p_SkipObject)
			continue;

		// Set subroutines
		GLuint uiSubRoutines[3] = { uiEmissiveSubs[p_Object->m_fEmissive != 0.0f],
		uiRefractSubs[p_Object->m_bTransparent],
		uiReflectSubs[p_Object->m_uiReflective] };
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 3, uiSubRoutines);

		// Bind VAO
		glBindVertexArray(p_Object->m_uiVAO);

		// Bind the Transform UBO
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, p_Object->m_uiTransformUBO);

		// Bind the textures to texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, p_Object->m_uiDiffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, p_Object->m_uiSpecular);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, p_Object->m_uiRough);
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, p_Object->m_uiNormal);
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, p_Object->m_uiBump);

		// If emissive then update uniform
		if (p_Object->m_fEmissive != 0.0f) {
			glUniform1f(1, p_Object->m_fEmissive);
		}

		// If transparent then update texture
		if (p_Object->m_bTransparent) {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_CUBE_MAP, g_uiSkyBox);
		}

		// If reflective then update texture and uniform
		if (p_Object->m_uiReflective == 1) {
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, p_Object->m_uiReflect);

			glBindBufferBase(GL_UNIFORM_BUFFER, 3, p_Object->m_uiReflectVPUBO);
		}
		else if (p_Object->m_uiReflective == 2) {
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_CUBE_MAP, p_Object->m_uiReflect);
		}

		//Set the parallax map scaling
		glUniform1f(3, p_Object->m_fBumpScale);

		// Draw the Object
		glDrawElements(GL_PATCHES, p_Object->m_uiNumIndices, GL_UNSIGNED_INT, 0);
	}
}

void GL_Render()
{
	// Generate shadows
	//GL_RenderShadows();

	// Generate reflection maps
	//for (unsigned i = 0; i < g_SceneData.m_uiNumReflecObjects; i++) {
	//    ReflectObjectData * p_RObject = &g_SceneData.mp_ReflecObjects[i];
	//    ObjectData * p_Object = &g_SceneData.mp_Objects[p_RObject->m_uiObjectPos];
	//
	//    // Check if cube reflection
	//    if (p_Object->m_uiReflective == 2) {
	//        // Calculate cube maps first as they recalculate planar reflections as well
	//        GL_RenderEnvironmentReflection(p_RObject, p_Object);
	//    }
	//}
	for (unsigned i = 0; i < g_SceneData.m_uiNumReflecObjects; i++) {
		ReflectObjectData* p_RObject = &g_SceneData.mp_ReflecObjects[i];
		ObjectData* p_Object = &g_SceneData.mp_Objects[p_RObject->m_uiObjectPos];

		// Check if planar or cube reflection
		if (p_Object->m_uiReflective == 1) {
			GL_RenderPlanarReflection(p_RObject, p_Object,
				g_SceneData.m_LocalCamera.m_v3Direction, cross(g_SceneData.m_LocalCamera.m_v3Right, g_SceneData.m_LocalCamera.m_v3Direction),
				g_SceneData.m_LocalCamera.m_v3Position, g_SceneData.m_LocalCamera.m_fFOV, g_SceneData.m_LocalCamera.m_fAspect,
				vec2(g_SceneData.m_LocalCamera.m_fNear, g_SceneData.m_LocalCamera.m_fFar));
		}
	}

	// Bind default camera
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, g_SceneData.m_uiCameraUBO);

	// Perform deferred render pass
	GL_RenderDeferred();

	// Perform final pass
	GL_RenderPostProcess();
}

void GL_Update(float fElapsedTime)
{
	// Update the cameras position
	g_SceneData.m_LocalCamera.m_v3Position += g_SceneData.m_LocalCamera.m_fMoveZ * fElapsedTime * g_SceneData.m_LocalCamera.m_v3Direction;
	g_SceneData.m_LocalCamera.m_v3Position += g_SceneData.m_LocalCamera.m_fMoveX * fElapsedTime * g_SceneData.m_LocalCamera.m_v3Right;

	// Determine rotation matrix for camera angles
	mat4 m4Rotation = rotate(mat4(1.0f), g_SceneData.m_LocalCamera.m_fAngleX, vec3(0.0f, 1.0f, 0.0f));
	m4Rotation = rotate(m4Rotation, g_SceneData.m_LocalCamera.m_fAngleY, vec3(1.0f, 0.0f, 0.0f));

	// Determine new view and right vectors
	g_SceneData.m_LocalCamera.m_v3Direction = mat3(m4Rotation) * vec3(0.0f, 0.0f, 1.0f);
	g_SceneData.m_LocalCamera.m_v3Right = mat3(m4Rotation) * vec3(-1.0f, 0.0f, 0.0f);

	// Create updated camera View matrix
	mat4 m4View = lookAt(
		g_SceneData.m_LocalCamera.m_v3Position,
		g_SceneData.m_LocalCamera.m_v3Position + g_SceneData.m_LocalCamera.m_v3Direction,
		cross(g_SceneData.m_LocalCamera.m_v3Right, g_SceneData.m_LocalCamera.m_v3Direction)
	);

	// Create updated camera projection matrix
	mat4 m4Projection = perspective(
		g_SceneData.m_LocalCamera.m_fFOV,
		g_SceneData.m_LocalCamera.m_fAspect,
		g_SceneData.m_LocalCamera.m_fNear, g_SceneData.m_LocalCamera.m_fFar
	);

	// Create updated ViewProjection matrix
	mat4 m4ViewProjection = m4Projection * m4View;

	// Calculate inverse view projection
	mat4 m4InvViewProjection = inverse(m4ViewProjection);

	// Create updated camera data
	CameraData Camera = {
	m4ViewProjection,
	g_SceneData.m_LocalCamera.m_v3Position,
	m4InvViewProjection };

	// Update the camera buffer
	glBindBuffer(GL_UNIFORM_BUFFER, g_SceneData.m_uiCameraUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), &Camera, GL_DYNAMIC_DRAW);
}

bool test::Init()
{
	rhi::Init();

	return GL_Init();
}
void test::Close()
{
	GL_Quit();
	ClearDefaultGraphicsResource();
	rhi::Close();
}

void test::Frame(double deltaTime)
{
	// Update the Scene
	GL_Update(deltaTime);

	// Render the scene
	GL_Render();
}