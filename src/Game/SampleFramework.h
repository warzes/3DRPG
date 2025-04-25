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

class SampleFramework
{
public:
	virtual ~SampleFramework();

	virtual void OnStart() = 0;
	virtual void OnClose() = 0;

	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnRender() = 0;
};