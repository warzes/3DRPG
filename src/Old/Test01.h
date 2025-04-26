#pragma once

#include "Core.h"
#include "Scene.h"
#include "Context.h"

class Test01 final
{
public:
	bool Init();
	void Close();

	void FixedUpdate(double deltaTime);
	void Frame(double deltaTime);
	void DrawImGui(double deltaTime);

	void ProcessInput(Camerao& camera, float deltaTime, bool& firstMouse, float& lastX, float& lastY);
};