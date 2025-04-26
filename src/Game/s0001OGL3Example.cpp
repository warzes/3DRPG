#include "s0001OGL3Example.h"
//=============================================================================
namespace
{
	const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}
)";

	const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
	FragColor = texture(texture1, TexCoord);
}
)";
}
//=============================================================================
s0001OGL3Example::~s0001OGL3Example()
{
}
//=============================================================================
void s0001OGL3Example::OnStart()
{
	
}
//=============================================================================
void s0001OGL3Example::OnResize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
}
//=============================================================================
void s0001OGL3Example::OnUpdate(float deltaTime)
{

}
//=============================================================================
void s0001OGL3Example::OnRender()
{
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
//=============================================================================
void s0001OGL3Example::OnImGuiDraw()
{

}
//=============================================================================