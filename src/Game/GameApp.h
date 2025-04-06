#pragma once

#include "CoreApp.h"
#include "Scene.h"
#include "Context.h"

bool InitGame();
void CloseGame();

void FixedUpdate(double deltaTime);
void FrameGame(double deltaTime);
void DrawImGui(double deltaTime);

void ProcessInput(Camera& camera, float deltaTime, bool& firstMouse, float& lastX, float& lastY);