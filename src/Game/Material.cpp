#include "stdafx.h"
#include "Material.h"
//=============================================================================
std::unordered_map<std::string, std::weak_ptr<Material>> Material::m_cache;
//=============================================================================
std::unordered_map<std::string, std::weak_ptr<Texture2D>> Material::m_textureCache;
//=============================================================================
static uint32_t gLastMatIdx = 0;
//=============================================================================
Material::Ptr Material::Load(
	const std::vector<std::string>& textures,
	const int32_t& albedoIdx,
	const int32_t& normalIdx,
	const glm::ivec2& roughnessIdx,
	const glm::ivec2& metallicIdx,
	const int32_t& emissiveIdx)
{
	std::string matId;

	if (textures.empty())
		matId = "Untextured_Material_" + std::to_string(rand());
	else
	{
		for (auto path : textures)
			matId += path;
	}

	if (m_cache.find(matId) == m_cache.end() || m_cache[matId].expired())
	{
		auto mat = std::shared_ptr<Material>(new Material(
			textures,
			albedoIdx,
			normalIdx,
			roughnessIdx,
			metallicIdx,
			emissiveIdx));
		m_cache[matId] = mat;
		return mat;
	}
	else
		return m_cache[matId].lock();
}
//=============================================================================
Material::Ptr Material::Create(glm::vec4 albedo, float roughness, float metallic, glm::vec3 emissive)
{
	auto mat = std::shared_ptr<Material>(new Material());

	mat->m_albedoColor = albedo;
	mat->m_roughness = roughness;
	mat->m_metallic = metallic;
	mat->m_emissiveColor = emissive;

	return mat;
}
//=============================================================================
Material::Material()
{
	m_id = gLastMatIdx++;
}
//=============================================================================
bool Material::IsLoaded(const std::string& name)
{
	return m_cache.find(name) != m_cache.end();
}
//=============================================================================
Material::Material(const std::vector<std::string>& textures, const int32_t& albedo_idx, const int32_t& normal_idx, const glm::ivec2& roughness_idx, const glm::ivec2& metallic_idx, const int32_t& emissive_idx) :
	m_roughnessChannel(roughness_idx.y), m_metallicChannel(metallic_idx.y)
{
	m_id = gLastMatIdx++;

	if (albedo_idx != -1 && textures[albedo_idx].size() > 0)
	{
		m_albedoIdx = m_textures.size();
		m_textures.push_back(loadTexture(textures[albedo_idx], true));
	}

	if (normal_idx != -1 && textures[normal_idx].size() > 0)
	{
		m_normalIdx = m_textures.size();
		m_textures.push_back(loadTexture(textures[normal_idx], false));
	}

	if (roughness_idx.x != -1 && textures[roughness_idx.x].size() > 0)
	{
		m_roughnessIdx = m_textures.size();
		m_textures.push_back(loadTexture(textures[roughness_idx.x], false));
	}

	if (metallic_idx.x != -1 && textures[metallic_idx.x].size() > 0)
	{
		m_metallicIdx = m_textures.size();
		m_textures.push_back(loadTexture(textures[metallic_idx.x], false));
	}

	if (emissive_idx != -1 && textures[emissive_idx].size() > 0)
	{
		m_emissiveIdx = m_textures.size();
		m_textures.push_back(loadTexture(textures[emissive_idx], false));
	}
}
//=============================================================================
Texture2D::Ptr Material::loadTexture(const std::string& path, bool srgb)
{
	if (m_textureCache.find(path) != m_textureCache.end() && !m_textureCache[path].expired())
		return m_textureCache[path].lock();
	else
	{
		auto tex = Texture2D::CreateFromFile(path, false, srgb);
		m_textureCache[path] = tex;
		return tex;
	}
}
//=============================================================================