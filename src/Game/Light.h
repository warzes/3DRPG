#pragma once

enum class LightType : uint8_t
{
	Point,
	Spot,
	Dir
};

class Light final
{
public:
	Light();
	Light(
		const glm::vec3& color,
		const glm::vec3& position,
		const glm::vec3& direction,
		float intense,
		LightType type);
	Light(
		const glm::vec3& color,
		const glm::vec3& position,
		const glm::vec3& direction,
		float intense,
		float cut,
		float outCut,
		LightType type);

	LightType GetLightType();

	glm::vec3 GetColor();
	void SetColor(const glm::vec3& color);

	glm::vec3 GetPosition();
	void SetPosition(const glm::vec3& pos);

	glm::vec3 GetDirection();
	void SetDirection(const glm::vec3& dir);

	float GetIntensity();
	void SetIntensity(float intense);

	float GetCutOff();
	void SetCutOff(float val);

	float GetOuterCutOff();
	void SetOuterCutOff(float val);

	void SetLightIndex(int index);

private:
	LightType m_lightType = LightType::Point;
	int m_lightIndexByType; // For putting this light into the correct array index in the shader. Each type has a seperate index that is kept track of by the scene

	float m_intensity;
	glm::vec3 m_lightColor;

	glm::vec3 m_position;
	glm::vec3 m_direction;

	//For spotlights
	float m_cutOff;
	float m_outerCutOff;

};