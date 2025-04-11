#include "stdafx.h"
#include "Render.h"
#include "CoreApp.h"
//=============================================================================
#if defined(_DEBUG)
void APIENTRY DebugCallback(uint32_t uiSource, uint32_t uiType, uint32_t /*uiID*/, uint32_t uiSeverity, int32_t /*iLength*/, const char* cMessage, void* /*userParam*/) noexcept
{
	std::string severity;
	switch (uiSeverity)
	{
	case GL_DEBUG_SEVERITY_HIGH:   severity = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM: severity = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW:    severity = "Low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
	default: severity = "Notification"; break;
	}

	std::string type;
	switch (uiType)
	{
	case GL_DEBUG_TYPE_ERROR:               type = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type = "Deprecated"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type = "Undefined"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         type = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         type = "Performance"; break;
	case GL_DEBUG_TYPE_OTHER:
	default: type = "Other"; break;
	}

	std::string source;
	switch (uiSource)
	{
	case GL_DEBUG_SOURCE_API:             source = "OpenGL"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source = "OS"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: source = "GLSL Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     source = "3rd Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     source = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER:
	default:
		source = "Other"; break;
	}

	const std::string message = "OpenGL Debug: Severity=" + severity + ", Type=" + type + ", Source=" + source + " - " + cMessage;

	if (uiSeverity == GL_DEBUG_SEVERITY_HIGH)
		Fatal(message);
	else
		Error(message);
}
#endif
//=============================================================================
void rhi::Init()
{
#if defined(_DEBUG)
	//Allow for synchronous callbacks.
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	//Set up the debug info callback
	glDebugMessageCallback((GLDEBUGPROC)&DebugCallback, nullptr);

	//Set up the type of debug information we want to receive
	uint32_t uiUnusedIDs = 0;
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &uiUnusedIDs, GL_TRUE); //Enable all
	//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE); //Disable notifications
	//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, NULL, GL_FALSE); //Disable low severity
	//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, NULL, GL_FALSE); //Disable medium severity warnings
#endif

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
//=============================================================================
void rhi::Close()
{

}