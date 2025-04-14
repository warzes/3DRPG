#pragma once

#include "CoreApp.h"
#include "Scene.h"
#include "Context.h"

namespace game
{
	bool Init();
	void Close();

	void FixedUpdate(double deltaTime);
	void Frame(double deltaTime);
	void DrawImGui(double deltaTime);

	void ProcessInput(Camera& camera, float deltaTime, bool& firstMouse, float& lastX, float& lastY);
}