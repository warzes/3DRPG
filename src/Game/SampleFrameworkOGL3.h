#pragma once

#define _USE_MATH_DEFINES 

#include <cmath>
#include <cstdint>

#include <string>
#include <vector>

#include <glad/gl.h>

#include <glfw/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>

//#define GLM_FORCE_XYZW_ONLY
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>

#include <stb/stb_image.h>
#include <gli/gli.hpp>

#include "CoreGL3.h"
#include "TempMathGL3.h"
#include "SimpleOpenGL3.h"


class SampleFrameworkOGL3
{
public:
	virtual ~SampleFrameworkOGL3();

	void Run(uint32_t width, uint32_t height);

	virtual void OnStart() = 0;

	virtual void OnResize(uint32_t width, uint32_t height) = 0;
	virtual void OnMouseMove(int xpos, int ypos) = 0;

	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnRender() = 0;
	virtual void OnImGuiDraw() = 0;

protected:
	GLFWwindow* m_window{ nullptr };
};

void StartSampleOGL3(uint32_t width, uint32_t height, SampleFrameworkOGL3* example);