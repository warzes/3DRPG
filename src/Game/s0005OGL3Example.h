#pragma once

#include "SampleFrameworkOGL3.h"

// загрузка меша

class s0005OGL3Example final : public SampleFrameworkOGL3
{
public:
	~s0005OGL3Example();

	void OnStart() final;

	void OnResize(uint32_t width, uint32_t height) final;
	void OnMouseMove(int xpos, int ypos) final;

	void OnUpdate(float deltaTime) final;
	void OnRender() final;
	void OnImGuiDraw() final;
};