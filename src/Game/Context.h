#pragma once

struct ContextCreateInfo final
{
	struct Window
	{
		uint32_t width{ 1600 };
		uint32_t height{ 900 };
		std::string_view title{ "Game" };
	} window;
};

class Context final
{
public:
	bool Init(const ContextCreateInfo& createInfo);
	void Close();

	bool ShouldClose() const;

	void BeginFrame();
	void BeginImgui();
	void EndImgui();
	void EndFrame();

	bool IsResize() const;

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	float GetAspect() const;
	double GetDeltaTime() const;
	GLFWwindow* GetWindow();

	glm::uvec2 GetCursorPosition() const;
	void SetCursorPosition(const glm::uvec2& position);

private:
	friend void SetWindowSize(int width, int height);

	GLFWwindow* m_window{ nullptr };
	uint32_t    m_frameWidth{ 0 };
	uint32_t    m_frameHeight{ 0 };
	float       m_screenAspect{ 0.0 };
	bool        m_isResize{ true };

	double      m_lastFrameTime{ 0.0 };
	double      m_deltaTime{ 0.0 };
};

float GetFrameAspect();
uint32_t GetFrameWidth();
uint32_t GetFrameHeight();
GLFWwindow* GetWindow();
Context* GetContext();