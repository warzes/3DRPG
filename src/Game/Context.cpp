#include "stdafx.h"
#include "Context.h"
#include "CoreApp.h"
//=============================================================================
Context* thisContext{ nullptr };
//=============================================================================
void SetWindowSize(int width, int height)
{
	assert(thisContext);

	width = std::max(width, 1);
	height = std::max(height, 1);

	if (thisContext->m_frameWidth != width || thisContext->m_frameHeight != height)
	{
		thisContext->m_frameWidth = width;
		thisContext->m_frameHeight = height;
		thisContext->m_screenAspect = (float)width / (float)height;
		thisContext->m_isResize = true;
	}
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
void handleKeyEvents(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept
{
	//std::string actionName;
	//switch (action)
	//{
	//case GLFW_PRESS:
	//	actionName = "pressed";
	//	break;
	//case GLFW_RELEASE:
	//	actionName = "released";
	//	break;
	//case GLFW_REPEAT:
	//	actionName = "repeated";
	//	break;
	//default:
	//	actionName = "invalid";
	//	break;
	//}

	//std::string keyName = glfwGetKeyName(key, 0);
	//Print(keyName + " - " + actionName);
}
//=============================================================================
void handleMouseButtonEvents(GLFWwindow* window, int button, int action, int mods) noexcept
{
	//std::string actionName;
	//switch (action)
	//{
	//case GLFW_PRESS:
	//	actionName = "pressed";
	//	break;
	//case GLFW_RELEASE:
	//	actionName = "released";
	//	break;
	//case GLFW_REPEAT:
	//	actionName = "repeated";
	//	break;
	//default:
	//	actionName = "invalid";
	//	break;
	//}

	//std::string mouseButtonName;
	//switch (button)
	//{
	//case GLFW_MOUSE_BUTTON_LEFT:
	//	mouseButtonName = "left";
	//	break;
	//case GLFW_MOUSE_BUTTON_MIDDLE:
	//	mouseButtonName = "middle";
	//	break;
	//case GLFW_MOUSE_BUTTON_RIGHT:
	//	mouseButtonName = "right";
	//	break;
	//default:
	//	mouseButtonName = "other";
	//	break;
	//}

	//Print(mouseButtonName + " - " + actionName);
}
//=============================================================================
void handleMousePositionEvents(GLFWwindow* window, double xpos, double ypos) noexcept
{

}
//=============================================================================
void handleMouseEnterLeaveEvents(GLFWwindow* window, int entered) noexcept
{

}
//=============================================================================
void handleFramebufferResizeEvents([[maybe_unused]] GLFWwindow* window, int width, int height) noexcept
{
	SetWindowSize(width, height);
}
//=============================================================================
bool Context::Init(int windowWidth, int windowHeight, std::string_view title)
{
	glfwSetErrorCallback([](int e, const char* str) { Fatal("GLTF Context error(" + std::to_string(e) + "): " + str); });

	if (!glfwInit())
	{
		Fatal("glfwInit() failed!");
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	m_window = glfwCreateWindow(windowWidth, windowHeight, title.data(), nullptr, nullptr);
	if (!m_window)
	{
		Fatal("Failed to create GLFW window!");
		return false;
	}

	glfwGetFramebufferSize(m_window, &m_frameWidth, &m_frameHeight);
	m_screenAspect = (float)m_frameWidth / (float)m_frameHeight;

	glfwSetWindowIconifyCallback(m_window, handleWindowMinimizedEvents);
	glfwSetWindowMaximizeCallback(m_window, handleWindowMaximizedEvents);
	glfwSetKeyCallback(m_window, handleKeyEvents);
	glfwSetMouseButtonCallback(m_window, handleMouseButtonEvents);
	glfwSetCursorPosCallback(m_window, handleMousePositionEvents);
	glfwSetCursorEnterCallback(m_window, handleMouseEnterLeaveEvents);
	glfwSetFramebufferSizeCallback(m_window, handleFramebufferResizeEvents);

	glfwMakeContextCurrent(m_window);

	if (!gladLoadGL(glfwGetProcAddress))
	{
		Fatal("Failed to initialize OpenGL context!");
		return false;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	ImGui::StyleColorsDark();

	glViewport(0, 0, m_frameWidth, m_frameHeight);

	m_lastFrameTime = glfwGetTime();

	thisContext = this;
	return true;
}
//=============================================================================
void Context::Close()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(m_window);
	glfwTerminate();
	thisContext = nullptr;
}
//=============================================================================
bool Context::ShouldClose() const
{
	return glfwWindowShouldClose(m_window);
}
//=============================================================================
bool Context::IsResize() const
{
	return m_isResize;
}
//=============================================================================
int Context::GetWidth() const
{
	return m_frameWidth;
}
//=============================================================================
int Context::GetHeight() const
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
	const double currentTime = glfwGetTime();
	m_deltaTime = currentTime - m_lastFrameTime;
	m_lastFrameTime = currentTime;
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
int GetFrameWidth()
{
	return thisContext->GetWidth();
}
//=============================================================================
int GetFrameHeight()
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