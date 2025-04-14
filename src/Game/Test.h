#pragma once

namespace test
{
	bool Init();
	void Close();

	inline void FixedUpdate(double deltaTime) {}
	void Frame(double deltaTime);
	inline void DrawImGui(double deltaTime) {}
}