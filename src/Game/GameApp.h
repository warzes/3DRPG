#pragma once

#include "CoreApp.h"
#include "Scene.h"


extern float GetFrameAspect();
extern int GetFrameWidth();
extern int GetFrameHeight();
extern bool IsResize();

bool InitGame();
void CloseGame();

void FixedUpdate(double deltaTime);
void FrameGame(double deltaTime);
void DrawImGui(double deltaTime);

void ProcessInput(Camera& camera, float deltaTime, bool& firstMouse, float& lastX, float& lastY);