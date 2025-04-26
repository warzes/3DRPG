#include "stdafx.h"
#include "RoguelikeGameApp.h"
#include "Context.h"
#include "Core.h"
//=============================================================================
// пока оставлю opengl 4.6. под браузеры сделаю отдельную версию рендера
//=============================================================================
RoguelikeGameApp::RoguelikeGameApp(Context& context)
	: m_context(context)
{

}
//=============================================================================
bool RoguelikeGameApp::Init()
{
	return true;
}
//=============================================================================
void RoguelikeGameApp::Close()
{

}
//=============================================================================
void RoguelikeGameApp::Resize(uint32_t width, uint32_t height)
{
	glViewport(0, 0, width, height);
}
//=============================================================================
void RoguelikeGameApp::Update(double deltaTime)
{

}
//=============================================================================
void RoguelikeGameApp::Draw(double deltaTime)
{

}
//=============================================================================
void RoguelikeGameApp::DrawImGui(double deltaTime)
{

}
//=============================================================================