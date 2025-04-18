#pragma once

#include "RHITexture.h"

class Material final
{
public:
	using Ptr = std::shared_ptr<Material>;

	// Material factory methods.
	static Material::Ptr Load(
		const std::vector<std::string>& textures,
		const int32_t& albedoIdx,
		const int32_t& normalIdx,
		const glm::ivec2& roughnessIdx,
		const glm::ivec2& metallicIdx,
		const int32_t& emissiveIdx);

	// Custom factory method for creating a material from provided data.
	static Material::Ptr Create(
		glm::vec4 albedo = glm::vec4(1.0f),
		float     roughness = 0.0f,
		float     metalness = 0.0f,
		glm::vec3 emissive = glm::vec3(0.0f));

	static bool IsLoaded(const std::string& name);

	auto Id() const { return m_id; }
	auto AlbedoValue() const { return m_albedoColor; }
	auto RoughnessValue() const { return m_roughness; }
	auto MetallicValue() const { return m_metallic; }
	auto EmissiveValue() const { return m_emissiveColor; }
	auto AlphaTest() const { return m_alphaTest; }

	auto AlbedoIdx() const { return m_albedoIdx; }
	auto NormalIdx() const { return m_normalIdx; }
	auto RoughnessIdx() const { return m_roughnessIdx; }
	auto MetallicIdx() const { return m_metallicIdx; }
	auto EmissiveIdx() const { return m_emissiveIdx; }
	auto RoughnessChannel() const { return m_roughnessChannel; }
	auto MetallicChannel() const { return m_metallicChannel; }

	void SetAlbedo(const glm::vec4& value) { m_albedoColor = value; }
	void SetRoughness(const float& value) { m_roughness = value; }
	void SetMetallic(const float& value) { m_metallic = value; }
	void SetEmissive(const glm::vec3& value) { m_emissiveColor = value; }
	void SetAlphaTest(const bool& value) { m_alphaTest = value; }

	auto AlbedoTexture() { return m_albedoIdx != -1 ? m_textures[m_albedoIdx] : nullptr; }
	auto NormalTexture() { return m_normalIdx != -1 ? m_textures[m_normalIdx] : nullptr; }
	auto RoughnessTexture() { return m_roughnessIdx != -1 ? m_textures[m_roughnessIdx] : nullptr; }
	auto MetallicTexture() { return m_metallicIdx != -1 ? m_textures[m_metallicIdx] : nullptr; }
	auto EmissiveTexture() { return m_emissiveIdx != -1 ? m_textures[m_emissiveIdx] : nullptr; }

private:
	static Texture2D::Ptr loadTexture(const std::string& path, bool srgb = false);

	Material(
		const std::vector<std::string>& textures,
		const int32_t& albedoIdx,
		const int32_t& normalIdx,
		const glm::ivec2& roughnessIdx,
		const glm::ivec2& metallicIdx,
		const int32_t& emissiveIdx);
	Material();

	// Material cache.
	static std::unordered_map<std::string, std::weak_ptr<Material>> m_cache;

	int32_t   m_albedoIdx = -1;
	int32_t   m_normalIdx = -1;
	int32_t   m_roughnessIdx = -1;
	int32_t   m_metallicIdx = -1;
	int32_t   m_emissiveIdx = -1;
	int32_t   m_roughnessChannel = -1;
	int32_t   m_metallicChannel = -1;
	glm::vec4 m_albedoColor = glm::vec4(1.0f);
	glm::vec3 m_emissiveColor = glm::vec3(0.0f);
	float     m_roughness = 1.0f;
	float     m_metallic = 0.0f;
	bool      m_alphaTest = false;
	uint32_t  m_id = 0;

	// Texture list. In the same order as the Assimp texture enums.
	std::vector<Texture2D::Ptr> m_textures;

	// Texture cache.
	static std::unordered_map<std::string, std::weak_ptr<Texture2D>> m_textureCache;
};