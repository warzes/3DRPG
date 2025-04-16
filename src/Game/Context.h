#pragma once

struct ContextCreateInfo final
{
	struct Window
	{
		uint32_t width{ 1600 };
		uint32_t height{ 900 };
		std::string_view title{ "Game" };

		bool maximized{ false };
		bool fullscreen{ false };
	} window;

	struct Render
	{
		bool vsync{ false };
		bool srgb{ false };
	} render;
};

constexpr size_t MaxKeys = 1024;
constexpr size_t MaxMouseButtons = 5;

class Context final
{
public:
	bool Init(const ContextCreateInfo& createInfo);
	void Close();

	bool ExitRequested() const;

	void BeginFrame();
	void BeginImgui();
	void EndImgui();
	void EndFrame();

	bool IsResize() const;

	double GetTimeInSeconds() const;

	uint32_t    GetWidth() const;
	uint32_t    GetHeight() const;
	float       GetAspect() const;
	double      GetDeltaTime() const;
	GLFWwindow* GetWindow();

	glm::uvec2 GetCursorPosition() const;
	void SetCursorPosition(const glm::uvec2& position);

	std::function<void(int code)> KeyPressedFunc{ nullptr };
	std::function<void(int code)> KeyReleasedFunc{ nullptr };
	std::function<void(double x, double y, double deltaX, double deltaY)> MouseMoveFunc{ nullptr };
	std::function<void(double xoffset, double yoffset)> MouseScrolledFunc{ nullptr };

private:
	friend void handleFramebufferResizeEvents(GLFWwindow*, int, int) noexcept;
	friend void handleKeyEvents(GLFWwindow*, int, int, int, int) noexcept;
	friend void handleMousePositionEvents(GLFWwindow*, double, double) noexcept;
	friend void handleMouseScrollEvents(GLFWwindow*, double, double) noexcept;

	GLFWwindow* m_window{ nullptr };
	uint32_t    m_frameWidth{ 0 };
	uint32_t    m_frameHeight{ 0 };
	float       m_screenAspect{ 0.0 };
	bool        m_isResize{ true };

	double      m_lastFrameTime{ 0.0 };
	double      m_deltaTime{ 0.0 };

	double      m_mouseX{ 0.0 };
	double      m_mouseY{ 0.0 };
	double      m_lastMouseX{ 0.0 };
	double      m_lastMouseY{ 0.0 };
	double      m_mouseDeltaX{ 0.0 };
	double      m_mouseDeltaY{ 0.0 };

	std::array<bool, MaxKeys> m_keys{ false };
	std::array<bool, MaxMouseButtons> m_mouseButtons{ false };
};

uint32_t    GetFrameWidth();
uint32_t    GetFrameHeight();
float       GetFrameAspect();

GLFWwindow* GetWindow();
Context*    GetContext();