#pragma once

#include "SampleFrameworkOGL3.h"

// spot light

class s0010OGL3Example final : public SampleFrameworkOGL3
{
public:
	~s0010OGL3Example();

	void OnStart() final;

	void OnResize(uint32_t width, uint32_t height) final;
	void OnMouseMove(int xpos, int ypos) final;

	void OnUpdate(float deltaTime) final;
	void OnRender() final;
	void OnImGuiDraw() final;
};