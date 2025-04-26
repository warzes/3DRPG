#pragma once

#include "SampleFrameworkOGL4.h"

class s0001OGL4Example final : public SampleFrameworkOGL4
{
public:
	~s0001OGL4Example();

	void OnStart() final;

	void OnResize(uint32_t width, uint32_t height) final;
	void OnUpdate(float deltaTime) final;
	void OnRender() final;
	void OnImGuiDraw() final;
};