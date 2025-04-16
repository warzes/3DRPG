#include "stdafx.h"
#include "Context.h"
#include "Core.h"
#include "Profiler.h"
//=============================================================================
Context* thisContext{ nullptr };
//=============================================================================
uint32_t GetFrameWidth()
{
	return thisContext->GetWidth();
}
//=============================================================================
uint32_t GetFrameHeight()
{
	return thisContext->GetHeight();
}
//=============================================================================
float GetFrameAspect()
{
	return thisContext->GetAspect();
}
//=============================================================================
GLFWwindow* GetWindow()
{
	return thisContext->GetWindow();
}
//=============================================================================
Context* GetContext()
{
	return thisContext;
}
//=============================================================================
void handleWindowMinimizedEvents(GLFWwindow* window, int minimized) noexcept
{

}
//=============================================================================
void handleWindowMaximizedEvents(GLFWwindow* window, int maximized) noexcept
{

}
//=============================================================================
void handleMouseEnterLeaveEvents(GLFWwindow* window, int entered) noexcept
{

}
//=============================================================================
void handleKeyEvents(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

	if (key >= 0 && key < MaxKeys)
	{
		if (action == GLFW_PRESS)
		{
			if (thisContext->KeyPressedFunc)
				thisContext->KeyPressedFunc(key);
			thisContext->m_keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			if (thisContext->KeyReleasedFunc)
				thisContext->KeyReleasedFunc(key);
			thisContext->m_keys[key] = false;
		}
		else if (action == GLFW_REPEAT)
		{
			// TODO:
		}
	}
	//std::string keyName = glfwGetKeyName(key, 0);
}
//=============================================================================
void handleMouseButtonEvents(GLFWwindow* window, int button, int action, int mods) noexcept
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

	if (button >= 0 && button < MaxMouseButtons)
	{
		if (action == GLFW_PRESS)
		{
			if (thisContext->MousePressedFunc)
				thisContext->MousePressedFunc(button);
			thisContext->m_mouseButtons[button] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			if (thisContext->MouseReleasedFunc)
				thisContext->MouseReleasedFunc(button);
			thisContext->m_mouseButtons[button] = false;
		}
	}
}
//=============================================================================
void handleMousePositionEvents([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) noexcept
{
	thisContext->m_mouseX = xpos;
	thisContext->m_mouseY = ypos;
	if (thisContext->MouseMoveFunc)
		thisContext->MouseMoveFunc(xpos, ypos, thisContext->m_mouseDeltaX, thisContext->m_mouseDeltaY);
}
//=============================================================================
void handleMouseScrollEvents(GLFWwindow* window, double xoffset, double yoffset) noexcept
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	if (thisContext->MouseScrolledFunc)
		thisContext->MouseScrolledFunc(xoffset, yoffset);
}
//=============================================================================
void handleCharEvents(GLFWwindow* window, unsigned int c) noexcept
{
	ImGui_ImplGlfw_CharCallback(window, c);
}
//=============================================================================
void handleWindowResizeEvents([[maybe_unused]] GLFWwindow* window, int width, int height) noexcept
{
	assert(width > 0);
	assert(height > 0);
	assert(thisContext);

	if (width < 0 || height < 0) return;
	width = std::max(width, 1);
	height = std::max(height, 1);

	if (thisContext->m_frameWidth != width || thisContext->m_frameHeight != height)
	{
		thisContext->m_frameWidth = width;
		thisContext->m_frameHeight = height;
		thisContext->m_screenAspect = static_cast<float>(width) / static_cast<float>(height);
		thisContext->m_isResize = true;
	}
}
//=============================================================================
bool Context::Init(const ContextCreateInfo& createInfo)
{
	glfwSetErrorCallback([](int e, const char* str) { Fatal("GLTF Context error(" + std::to_string(e) + "): " + str); });

	if (!glfwInit())
	{
		Fatal("Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_MAXIMIZED, createInfo.window.maximized ? GL_TRUE : GL_FALSE);

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#if defined(__EMSCRIPTEN__)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#else
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif

#if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
	glfwWindowHint(GLFW_CONTEXT_NO_ERROR, GLFW_TRUE);
#endif

	m_window = glfwCreateWindow(createInfo.window.width, createInfo.window.height, createInfo.window.title.data(), createInfo.window.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	if (!m_window)
	{
		Fatal("Failed to create GLFW window!");
		return false;
	}

	glfwSetWindowIconifyCallback(m_window, handleWindowMinimizedEvents);
	glfwSetWindowMaximizeCallback(m_window, handleWindowMaximizedEvents);
	glfwSetCursorEnterCallback(m_window, handleMouseEnterLeaveEvents);
	
	glfwSetKeyCallback(m_window, handleKeyEvents);
	glfwSetCursorPosCallback(m_window, handleMousePositionEvents);
	glfwSetScrollCallback(m_window, handleMouseScrollEvents);
	glfwSetMouseButtonCallback(m_window, handleMouseButtonEvents);
	glfwSetCharCallback(m_window, handleCharEvents);
	glfwSetWindowSizeCallback(m_window, handleWindowResizeEvents);

	glfwMakeContextCurrent(m_window);

	if (!gladLoadGL(glfwGetProcAddress))
	{
		Fatal("Failed to initialize OpenGL context!");
		return false;
	}

	glfwSwapInterval(createInfo.render.vsync ? 1 : 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(m_window, false);
	ImGui_ImplOpenGL3_Init("#version 150");
	ImGui::StyleColorsDark();

	GLFWmonitor* primary = glfwGetPrimaryMonitor();

	float xscale, yscale;
	glfwGetMonitorContentScale(primary, &xscale, &yscale);

	ImGuiStyle* style = &ImGui::GetStyle();
	style->ScaleAllSizes(xscale > yscale ? xscale : yscale);

	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = xscale > yscale ? xscale : yscale;
	io.IniFilename = nullptr;

	int displayW, displayH;
	glfwGetFramebufferSize(m_window, &displayW, &displayH);
	if (displayW < 0 || displayH < 0)
	{
		Fatal("glfwGetFramebufferSize failed!");
		return false;
	}

	m_frameWidth = std::max(displayW, 1);
	m_frameHeight = std::max(displayH, 1);
	m_screenAspect = static_cast<float>(m_frameWidth) / static_cast<float>(m_frameHeight);
	glViewport(0, 0, m_frameWidth, m_frameHeight);

	m_lastFrameTime = glfwGetTime();

	profiler::Init();

	thisContext = this;
	return true;
}
//=============================================================================
void Context::Close()
{
	profiler::Close();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(m_window);
	glfwTerminate();
	thisContext = nullptr;
}
//=============================================================================
bool Context::ExitRequested() const
{
	return glfwWindowShouldClose(m_window);
}
//=============================================================================
bool Context::IsResize() const
{
	return m_isResize;
}
//=============================================================================
double Context::GetTimeInSeconds() const
{
	return glfwGetTime();
}
//=============================================================================
uint32_t Context::GetWidth() const
{
	return m_frameWidth;
}
//=============================================================================
uint32_t Context::GetHeight() const
{
	return m_frameHeight;
}
//=============================================================================
float Context::GetAspect() const
{
	return m_screenAspect;
}
//=============================================================================
double Context::GetDeltaTime() const
{
	return m_deltaTime;
}
//=============================================================================
GLFWwindow* Context::GetWindow()
{
	return m_window;
}
//=============================================================================
void Context::BeginFrame()
{
	const double currentTime = GetTimeInSeconds();
	m_deltaTime     = currentTime - m_lastFrameTime;
	m_lastFrameTime = currentTime;

	m_mouseDeltaX = m_mouseX - m_lastMouseX;
	m_mouseDeltaY = m_mouseY - m_lastMouseY;
	m_lastMouseX = m_mouseX;
	m_lastMouseY = m_mouseY;

	profiler::BeginFrame();
}
//=============================================================================
void Context::BeginImgui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
//=============================================================================
void Context::EndImgui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
//=============================================================================
void Context::EndFrame()
{
	profiler::EndFrame();
	m_isResize = false;
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
//=============================================================================
glm::uvec2 Context::GetCursorPosition() const
{	
	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);
	return glm::uvec2{ static_cast<glm::uint>(xpos), static_cast<glm::uint>(ypos) };
}
//=============================================================================
void Context::SetCursorPosition(const glm::uvec2& position)
{
	glfwSetCursorPos(m_window, static_cast<double>(position.x), static_cast<double>(position.y));
}
//=============================================================================