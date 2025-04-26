#include "stdafx.h"
#include "Light.h"
//=============================================================================
Light::Light() 
	: m_lightColor(glm::vec3(1.0f, 1.0f, 1.0f))
	, m_position(glm::vec3(0, 0, 0))
	, m_direction(glm::vec3(0, -90, 0))
	, m_intensity(0.5f)
	, m_lightType(LightType::Point)
{
}
//=============================================================================
Light::Light(const glm::vec3& col, const glm::vec3& pos, const glm::vec3& dir, float intense, LightType type) 
	: m_lightColor(col)
	, m_position(pos)
	, m_direction(dir)
	, m_intensity(intense)
	, m_lightType(type)
{
}
//=============================================================================
Light::Light(const glm::vec3& col, const glm::vec3& pos, const glm::vec3& dir, float intense, float cut, float outCut, LightType type) 
	: m_lightColor(col)
	, m_position(pos)
	, m_direction(dir)
	, m_intensity(intense)
	, m_cutOff(cut)
	, m_outerCutOff(outCut)
	, m_lightType(type)
{
}
//=============================================================================
LightType Light::GetLightType()
{
	return m_lightType;
}
//=============================================================================
glm::vec3 Light::GetColor() {
	return m_lightColor;
}
//=============================================================================
void Light::SetColor(const glm::vec3& color) {
	m_lightColor = color;
}
//=============================================================================
glm::vec3 Light::GetPosition()
{
	return m_position;
}
//=============================================================================
void Light::SetPosition(const glm::vec3& pos)
{
	m_position = pos;
}
//=============================================================================
glm::vec3 Light::GetDirection()
{
	return m_direction;
}
//=============================================================================
void Light::SetDirection(const glm::vec3& dir)
{
	m_direction = dir;
}
//=============================================================================
float Light::GetIntensity()
{
	return m_intensity;
}
//=============================================================================
void Light::SetIntensity(float intense)
{
	m_intensity = intense;
}
//=============================================================================
float Light::GetCutOff()
{
	return m_cutOff;
}
//=============================================================================
void Light::SetCutOff(float val)
{
	m_cutOff = val;
}
//=============================================================================
float Light::GetOuterCutOff()
{
	return m_outerCutOff;
}
//=============================================================================
void Light::SetOuterCutOff(float val)
{
	m_outerCutOff = val;
}
//=============================================================================
void Light::SetLightIndex(int index)
{
	m_lightIndexByType = index;
}
//=============================================================================