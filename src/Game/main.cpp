#include "stdafx.h"
#include "CoreApp.h"
#include "GameApp.h"
//=============================================================================
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#endif
//=============================================================================
namespace
{
	int frameWidth{ 0 };
	int frameHeight{ 0 };
	bool isResize{ true };
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
	std::string actionName;
	switch (action)
	{
	case GLFW_PRESS:
		actionName = "pressed";
		break;
	case GLFW_RELEASE:
		actionName = "released";
		break;
	case GLFW_REPEAT:
		actionName = "repeated";
		break;
	default:
		actionName = "invalid";
		break;
	}

	std::string keyName = glfwGetKeyName(key, 0);
	Print(keyName + " - " + actionName);
}
//=============================================================================
void handleMouseButtonEvents(GLFWwindow* window, int button, int action, int mods) noexcept
{
	std::string actionName;
	switch (action)
	{
	case GLFW_PRESS:
		actionName = "pressed";
		break;
	case GLFW_RELEASE:
		actionName = "released";
		break;
	case GLFW_REPEAT:
		actionName = "repeated";
		break;
	default:
		actionName = "invalid";
		break;
	}

	std::string mouseButtonName;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		mouseButtonName = "left";
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		mouseButtonName = "middle";
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		mouseButtonName = "right";
		break;
	default:
		mouseButtonName = "other";
		break;
	}

	Print(mouseButtonName + " - " + actionName);
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
void handleFramebufferResizeEvents(GLFWwindow* window, int width, int height) noexcept
{
	if (frameWidth != width || frameHeight != height)
	{
		frameWidth = width;
		frameHeight = height;
		isResize = true;
	}
}
//=============================================================================
bool InitApp(int windowWidth, int windowHeight)
{
	if (!glfwInit())
	{
		Fatal("glfwInit() failed!");
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	app::window = glfwCreateWindow(windowWidth, windowHeight, "Game", nullptr, nullptr);
	if (!app::window)
	{
		Fatal("Failed to create GLFW window!");
		return false;
	}

	int fbWidth, fbHeight;
	glfwGetFramebufferSize(app::window, &fbWidth, &fbHeight);
	frameWidth = fbWidth;
	frameHeight = fbHeight;

	glfwSetWindowIconifyCallback(app::window, handleWindowMinimizedEvents);
	glfwSetWindowMaximizeCallback(app::window, handleWindowMaximizedEvents);
	glfwSetKeyCallback(app::window, handleKeyEvents);
	glfwSetMouseButtonCallback(app::window, handleMouseButtonEvents);
	glfwSetCursorPosCallback(app::window, handleMousePositionEvents);
	glfwSetCursorEnterCallback(app::window, handleMouseEnterLeaveEvents);
	glfwSetFramebufferSizeCallback(app::window, handleFramebufferResizeEvents);

	glfwMakeContextCurrent(app::window);

	if (!gladLoadGL(glfwGetProcAddress))
	{
		Fatal("Failed to initialize OpenGL context!");
		return false;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(app::window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	ImGui::StyleColorsDark();

	return true;
}
//=============================================================================
bool ShouldCloseApp()
{
	return app::isExit || glfwWindowShouldClose(app::window);
}
//=============================================================================
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	if (InitApp(1600, 900) && InitGame())
	{
		double prevFrameStartTime = glfwGetTime();

		while (!ShouldCloseApp())
		{
			const double frameStartTime = glfwGetTime();
			const double deltaTime = frameStartTime - prevFrameStartTime;
			prevFrameStartTime = frameStartTime;

			if (isResize)
			{
				glViewport(0, 0, frameWidth, frameHeight);
				isResize = false;
			}

			FrameGame(deltaTime);

			// imgui
			{
				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				DrawImGui(deltaTime);

				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}

			glfwSwapBuffers(app::window);
			glfwPollEvents();
		}
	}
	CloseGame();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(app::window);
	glfwTerminate();
}
//=============================================================================