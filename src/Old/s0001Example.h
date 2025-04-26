#pragma once

#include "SampleFramework.h"

class s0001Example final : public SampleFramework
{
public:
	~s0001Example();

	void OnStart() final;

	void OnResize(uint32_t width, uint32_t height) final;
	void OnUpdate(float deltaTime) final;
	void OnRender() final;
	void OnImGuiDraw() final;
};