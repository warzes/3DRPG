#pragma once

#include "Camera.h"

class Context;

/*
терейн

*/

class _0002SimpleDemo final
{
public:
	_0002SimpleDemo(Context& context);

	bool Init();
	void Close();

	void Resize(uint32_t width, uint32_t height);

	void Update(double deltaTime);

	void Draw(double deltaTime);

	void DrawImGui(double deltaTime);

private:
	Context& m_context;

	Camera m_mainCamera;
};