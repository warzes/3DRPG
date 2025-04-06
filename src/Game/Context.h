#pragma once

class Context final
{
public:
	bool Init(int windowWidth, int windowHeight, std::string_view title);
	void Close();

	bool ShouldClose() const;

	void BeginFrame();
	void BeginImgui();
	void EndImgui();
	void EndFrame();

	bool IsResize() const;

	int GetWidth() const;
	int GetHeight() const;
	float GetAspect() const;
	double GetDeltaTime() const;
	GLFWwindow* GetWindow();

	glm::uvec2 GetCursorPosition() const;
	void SetCursorPosition(const glm::uvec2& position);

private:
	friend void SetWindowSize(int width, int height);

	GLFWwindow* m_window{ nullptr };
	int         m_frameWidth{ 0 };
	int         m_frameHeight{ 0 };
	float       m_screenAspect{ 0.0 };
	bool        m_isResize{ true };

	double      m_lastFrameTime{ 0.0 };
	double      m_deltaTime{ 0.0 };
};

float GetFrameAspect();
int GetFrameWidth();
int GetFrameHeight();
GLFWwindow* GetWindow();
Context* GetContext();