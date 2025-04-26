#pragma once

#include "SampleFrameworkOGL3.h"

class s0002OGL3Example final : public SampleFrameworkOGL3
{
public:
	~s0002OGL3Example();

	void OnStart() final;

	void OnResize(uint32_t width, uint32_t height) final;
	void OnUpdate(float deltaTime) final;
	void OnRender() final;
	void OnImGuiDraw() final;
};