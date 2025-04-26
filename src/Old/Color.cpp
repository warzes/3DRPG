#include "stdafx.h"
#include "Color.h"
//=============================================================================
Color3::Color3(uint8_t R, uint8_t G, uint8_t B)
{
	SetByte(R, G, B);
}
//=============================================================================
Color3::Color3(uint32_t dword)
{
	SetDWord(dword);
}
//=============================================================================
void Color3::SetByte(uint8_t R, uint8_t G, uint8_t B)
{
	const float scale = 1.0f / 255.0f;
	r = R * scale;
	g = G * scale;
	b = B * scale;
}
//=============================================================================
void Color3::GetByte(uint8_t& R, uint8_t& G, uint8_t& B) const
{
	R = glm::clamp(int(r * 255.0f), 0, 255);
	G = glm::clamp(int(g * 255.0f), 0, 255);
	B = glm::clamp(int(b * 255.0f), 0, 255);
}
//=============================================================================
void Color3::SetDWord(uint32_t color)
{
	const int R = color & 0xff;
	const int G = (color >> 8) & 0xff;
	const int B = (color >> 16) & 0xff;

	const float scale = 1.0f / 255.0f;
	r = R * scale;
	g = G * scale;
	b = B * scale;
}
//=============================================================================
uint32_t Color3::GetDWord() const
{
	const int R = glm::clamp(int(r * 255), 0, 255);
	const int G = glm::clamp(int(g * 255), 0, 255);
	const int B = glm::clamp(int(b * 255), 0, 255);
	const int A = 255;

	return R | (G << 8) | (B << 16) | (A << 24);
}
//=============================================================================
Color4::Color4(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
{
	SetByte(R, G, B, A);
}
//=============================================================================
Color4::Color4(uint32_t dword)
{
	SetDWord(dword);
}
//=============================================================================
void Color4::SetByte(uint8_t R, uint8_t G, uint8_t B, uint8_t A)
{
	const float scale = 1.0f / 255.0f;
	r = R * scale;
	g = G * scale;
	b = B * scale;
	a = A * scale;
}
//=============================================================================
void Color4::GetByte(uint8_t& R, uint8_t& G, uint8_t& B, uint8_t& A) const
{
	R = glm::clamp(int(r * 255.0f), 0, 255);
	G = glm::clamp(int(g * 255.0f), 0, 255);
	B = glm::clamp(int(b * 255.0f), 0, 255);
	A = glm::clamp(int(a * 255.0f), 0, 255);
}
//=============================================================================
void Color4::SetDWord(uint32_t color)
{
	const int R = color & 0xff;
	const int G = (color >> 8) & 0xff;
	const int B = (color >> 16) & 0xff;
	const int A = (color >> 24);

	const float scale = 1.0f / 255.0f;
	r = R * scale;
	g = G * scale;
	b = B * scale;
	a = A * scale;
}
//=============================================================================
uint32_t Color4::GetDWord() const
{
	const int R = glm::clamp(int(r * 255), 0, 255);
	const int G = glm::clamp(int(g * 255), 0, 255);
	const int B = glm::clamp(int(b * 255), 0, 255);
	const int A = glm::clamp(int(a * 255), 0, 255);

	return R | (G << 8) | (B << 16) | (A << 24);
}
//=============================================================================