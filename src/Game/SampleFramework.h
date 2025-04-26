#pragma once

#include <glad/gl.h>

#include <glfw/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define GLM_FORCE_XYZW_ONLY
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIAN
#include <glm/glm.hpp>

#include <stb/stb_image.h>
#include <stb/stb_truetype.h>

#include <tiny_obj_loader.h>

#include "SimpleOpenGL.h"

class SampleFramework
{
public:
	virtual ~SampleFramework();

	void Run(uint32_t width, uint32_t height);

	virtual void OnStart() = 0;

	virtual void OnResize(uint32_t width, uint32_t height) = 0;
	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnRender() = 0;
	virtual void OnImGuiDraw() = 0;

protected:
	GLFWwindow* m_window{ nullptr };
};

void StartSample(uint32_t width, uint32_t height, SampleFramework* example);