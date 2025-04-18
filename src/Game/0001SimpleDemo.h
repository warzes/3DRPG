#pragma once

class Context;

class _0001SimpleDemo final
{
public:
	_0001SimpleDemo(Context& context);

	bool Init();
	void Close();

	void Resize(uint32_t width, uint32_t height);

	void Update(double deltaTime);

	void Draw(double deltaTime);

	void DrawImGui(double deltaTime);

private:
	Context& m_context;
};