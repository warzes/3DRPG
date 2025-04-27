#include "SampleFrameworkOGL3.h"
//=============================================================================
namespace
{
	SampleFrameworkOGL3* thisSampleFramework{ nullptr };
}
//=============================================================================
void exampleOGL3HandleWindowResizeEvents([[maybe_unused]] GLFWwindow* window, int width, int height) noexcept
{
	thisSampleFramework->OnResize(width, height);
}
//=============================================================================
void exampleOGL3HandleMouseMoveEvents([[maybe_unused]] GLFWwindow* window, double xpos, double ypos) noexcept
{
	thisSampleFramework->OnMouseMove(xpos, ypos);
}
//=============================================================================
SampleFrameworkOGL3::~SampleFrameworkOGL3()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(m_window);
	glfwTerminate();

	thisSampleFramework = nullptr;
}
//=============================================================================
void SampleFrameworkOGL3::Run(uint32_t width, uint32_t height)
{
	thisSampleFramework = this;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#if defined(_DEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	m_window = glfwCreateWindow(width, height, "Example", nullptr, nullptr);

	glfwSetWindowSizeCallback(m_window, exampleOGL3HandleWindowResizeEvents);
	glfwSetCursorPosCallback(m_window, exampleOGL3HandleMouseMoveEvents);

	glfwMakeContextCurrent(m_window);
	gladLoadGL(glfwGetProcAddress);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(m_window, false);
	ImGui_ImplOpenGL3_Init("#version 150");
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;

	int displayW, displayH;
	glfwGetFramebufferSize(m_window, &displayW, &displayH);
	OnResize(displayW, displayH);

	OnStart();

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	// Основной цикл
	while (!glfwWindowShouldClose(m_window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		OnUpdate(deltaTime);

		OnRender();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		OnImGuiDraw();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}
//=============================================================================
void StartSampleOGL3(uint32_t width, uint32_t height, SampleFrameworkOGL3* example)
{
	try
	{
		example->Run(width, height);
	}
	catch (const std::exception& msg)
	{
		puts(msg.what());
	}
}
//=============================================================================