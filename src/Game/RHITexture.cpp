#include "stdafx.h"
#include "RHITexture.h"
#include "Core.h"
//=============================================================================
size_t PixelSizeFromType(GLenum type)
{
	if (type == GL_UNSIGNED_BYTE || type == GL_BYTE)
		return sizeof(uint8_t);
	else if (type == GL_HALF_FLOAT)
		return sizeof(uint16_t);
	else if (type == GL_FLOAT)
		return sizeof(float);
	else
		return 0;
}
//=============================================================================
int NumChannelsFromInternalFormat(GLenum fmt)
{
	if (fmt == GL_R8 || fmt == GL_R16F || fmt == GL_R32F)
		return 1;
	else if (fmt == GL_RG8 || fmt == GL_RG16F || fmt == GL_RG32F)
		return 2;
	else if (fmt == GL_RGB8 || fmt == GL_RGB16F || fmt == GL_RGB32F)
		return 3;
	else if (fmt == GL_RGBA8 || fmt == GL_RGBA16F || fmt == GL_RGBA32F)
		return 4;
	else
		return 0;
}
//=============================================================================
void TextureBase::Bind(uint32_t unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(m_target, m_glTex);
}
//=============================================================================
void TextureBase::Unbind(uint32_t unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(m_target, 0);
}
//=============================================================================
void TextureBase::BindImage(uint32_t unit, uint32_t mip_level, uint32_t layer, GLenum access, GLenum format)
{
	Bind(unit);
	if (m_arraySize > 1 || m_target == GL_TEXTURE_3D)
		glBindImageTexture(unit, m_glTex, mip_level, GL_TRUE, layer, access, format);
	else
		glBindImageTexture(unit, m_glTex, mip_level, GL_FALSE, 0, access, format);
}
//=============================================================================
void TextureBase::GenerateMipmaps()
{
	glGenerateTextureMipmap(m_glTex);
}
//=============================================================================
GLuint TextureBase::Id() const
{
	return m_glTex;
}
//=============================================================================
GLenum TextureBase::Target() const
{
	return m_target;
}
//=============================================================================
uint32_t TextureBase::ArraySize() const
{
	return m_arraySize;
}
//=============================================================================
uint32_t TextureBase::Version() const
{
	return m_version;
}
//=============================================================================
uint32_t TextureBase::MipLevels() const
{
	return m_mipLevels;
}
//=============================================================================
void TextureBase::SetWrapping(GLenum s, GLenum t, GLenum r)
{
	glTextureParameteri(m_glTex, GL_TEXTURE_WRAP_S, s);
	glTextureParameteri(m_glTex, GL_TEXTURE_WRAP_T, t);
	glTextureParameteri(m_glTex, GL_TEXTURE_WRAP_R, r);
}
//=============================================================================
void TextureBase::SetBorderColor(float r, float g, float b, float a)
{
	float borderColor[] = { r, g, b, a };
	glTextureParameterfv(m_glTex, GL_TEXTURE_BORDER_COLOR, borderColor);
}
//=============================================================================
void TextureBase::SetMinFilter(GLenum filter)
{
	glTextureParameteri(m_glTex, GL_TEXTURE_MIN_FILTER, filter);
}
//=============================================================================
void TextureBase::SetMagFilter(GLenum filter)
{
	glTextureParameteri(m_glTex, GL_TEXTURE_MAG_FILTER, filter);
}
//=============================================================================
void TextureBase::SetCompareMode(GLenum mode)
{
	glTextureParameteri(m_glTex, GL_TEXTURE_COMPARE_MODE, mode);
}
//=============================================================================
void TextureBase::SetCompareFunc(GLenum func)
{
	glTextureParameteri(m_glTex, GL_TEXTURE_COMPARE_FUNC, func);
}
//=============================================================================
bool TextureBase::IsCompressed(int mipLevel)
{
	GLint param = 0;
	glGetTextureLevelParameteriv(m_glTex, mipLevel, GL_TEXTURE_COMPRESSED, &param);
	return param == 1;
}
//=============================================================================
int TextureBase::CompressedSize(int mipLevel)
{
	GLint param = 0;
	glGetTextureLevelParameteriv(m_glTex, mipLevel, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &param);
	return param;
}
//=============================================================================
GLuint64 TextureBase::MakeTextureHandleResident()
{
	m_textureHandle = glGetTextureHandleARB(m_glTex);
	glMakeTextureHandleResidentARB(m_textureHandle);
	return m_textureHandle;
}
//=============================================================================
void TextureBase::MakeTextureHandleNonResident()
{
	if (m_textureHandle != 0)
	{
		glMakeTextureHandleNonResidentARB(m_textureHandle);
		m_textureHandle = 0;
	}
}
//=============================================================================
GLuint64 TextureBase::MakeImageHandleResident(GLenum access, GLint level, GLboolean layered, GLint layer)
{
	m_imageHandle = glGetImageHandleARB(m_glTex, level, layered, layer, m_format);
	glMakeImageHandleResidentARB(m_glTex, access);
	return m_imageHandle;
}
//=============================================================================
void TextureBase::MakeImageHandleNonResident()
{
	if (m_imageHandle != 0)
	{
		glMakeImageHandleNonResidentARB(m_imageHandle);
		m_imageHandle = 0;
	}
}
//=============================================================================
void TextureBase::SetName(const std::string& name)
{
	setName(m_glTex, name);
}
//=============================================================================
void TextureBase::closeTextureBase()
{
	MakeTextureHandleNonResident();
	MakeImageHandleNonResident();
	glDeleteTextures(1, &m_glTex);
}
//=============================================================================
void TextureBase::setName(const GLuint& name, const std::string& label)
{
	m_name = label;
	glObjectLabel(m_identifier, name, label.size(), label.c_str());
}
//=============================================================================
Texture1D::Texture1D(uint32_t w, uint32_t arraySize, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type)
{
	m_arraySize = arraySize;
	m_internalFormat = internalFormat;
	m_format = format;
	m_type = type;
	m_width = w;

	// If mip levels is -1, calculate mip levels
	if (mipLevels == -1)
	{
		m_mipLevels = 1;

		int width = m_width;

		while (width > 1)
		{
			width = std::max(1, (width / 2));
			m_mipLevels++;
		}
	}
	else
		m_mipLevels = mipLevels;

	allocate();

	// Default sampling options.
	SetWrapping(GL_REPEAT, GL_REPEAT, GL_REPEAT);
	SetMagFilter(GL_LINEAR);

	if (m_mipLevels > 1)
		SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	else
		SetMinFilter(GL_LINEAR);
}
//=============================================================================
Texture1D::~Texture1D()
{
	closeTextureBase();
}
//=============================================================================
Texture1D::Ptr Texture1D::Create(uint32_t w, uint32_t arraySize, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type)
{
	return std::shared_ptr<Texture1D>(new Texture1D(w, arraySize, mipLevels, internalFormat, format, type));
}
//=============================================================================
void Texture1D::WriteData(int arrayIndex, int mipLevel, void* data)
{
	int width = m_width;

	for (int i = 0; i < mipLevel; i++)
		width = std::max(1, width / 2);

	WriteSubData(arrayIndex, mipLevel, 0, width, data);
}
//=============================================================================
void Texture1D::WriteSubData(int arrayIndex, int mipLevel, int xOffset, int width, void* data)
{
	if (m_arraySize > 1)
		glTextureSubImage2D(m_glTex, mipLevel, xOffset, arrayIndex, width, 1, m_format, m_type, data);
	else
		glTextureSubImage1D(m_glTex, mipLevel, xOffset, width, m_format, m_type, data);
}
//=============================================================================
void Texture1D::WriteCompressedData(int arrayIndex, int mipLevel, int size, void* data)
{
	int width = m_width;

	for (int i = 0; i < mipLevel; i++)
		width = std::max(1, width / 2);

	WriteCompressedSubData(arrayIndex, mipLevel, 0, width, size, data);
}
//=============================================================================
void Texture1D::WriteCompressedSubData(int arrayIndex, int mipLevel, int xOffset, int width, int size, void* data)
{
	if (m_arraySize > 1)
		glCompressedTextureSubImage2D(m_glTex, mipLevel, xOffset, arrayIndex, width, 1, m_internalFormat, size, data);
	else
		glCompressedTextureSubImage1D(m_glTex, mipLevel, xOffset, width, m_internalFormat, size, data);
}
//=============================================================================
void Texture1D::Resize(uint32_t w)
{
	if (m_glTex != UINT32_MAX)
		glDeleteTextures(1, &m_glTex);

	m_version++;
	m_width = w;

	// Check if the max number of mip-levels possible with the current size is less than the earlier number, if so use the smaller number.
	int mip_levels = 1;

	int width = m_width;

	while (width > 1)
	{
		width = std::max(1, (width / 2));
		mip_levels++;
	}

	if (mip_levels < m_mipLevels)
		m_mipLevels = mip_levels;

	allocate();
}
//=============================================================================
uint32_t Texture1D::Width() const
{
	return m_width;
}
//=============================================================================
void Texture1D::allocate()
{
	// Allocate memory for mip levels.
	if (m_arraySize > 1)
	{
		m_target = GL_TEXTURE_1D_ARRAY;
		glCreateTextures(m_target, 1, &m_glTex);
		glTextureStorage2D(m_glTex, m_mipLevels, m_internalFormat, m_width, m_arraySize);
	}
	else
	{
		m_target = GL_TEXTURE_1D;
		glCreateTextures(m_target, 1, &m_glTex);
		glTextureStorage1D(m_glTex, m_mipLevels, m_internalFormat, m_width);
	}
}
//=============================================================================
Texture2D::Texture2D(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type)
{
	m_arraySize = arraySize;
	m_internalFormat = internalFormat;
	m_format = format;
	m_type = type;
	m_numSamples = numSamples;
	m_mipLevels = mipLevels;
	m_width = w;
	m_height = h;

	// If mip levels is -1, calculate mip levels
	if (m_mipLevels == -1)
	{
		m_mipLevels = 1;

		int width = m_width;
		int height = m_height;

		while (width > 1 || height > 1)
		{
			width = std::max(1, (width / 2));
			height = std::max(1, (height / 2));
			m_mipLevels++;
		}
	}

	allocate();

	// Default sampling options.
	SetWrapping(GL_REPEAT, GL_REPEAT, GL_REPEAT);
	SetMagFilter(GL_LINEAR);

	if (m_mipLevels > 1)
		SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	else
		SetMinFilter(GL_LINEAR);
}
//=============================================================================
Texture2D::~Texture2D()
{
	closeTextureBase();
}
//=============================================================================
Texture2D::Ptr Texture2D::Create(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type)
{
	return std::shared_ptr<Texture2D>(new Texture2D(w, h, arraySize, mipLevels, numSamples, internalFormat, format, type));
}
//=============================================================================
Texture2D::Ptr Texture2D::CreateFromFile(std::string path, bool flipVertical, bool srgb)
{
	stbi_set_flip_vertically_on_load(flipVertical);

	std::string ext = GetFileExtension(path);

	int x, y, n;
	if (ext == "hdr")
	{
		float* data = stbi_loadf(path.c_str(), &x, &y, &n, 0);
		if (!data)
		{
			Error("Texture File not find: " + path);
			return nullptr;
		}

		auto texture = Texture2D::Create(x, y, 1, -1, 1, GL_RGB32F, GL_RGB, GL_FLOAT);
		texture->WriteData(0, 0, data);
		texture->GenerateMipmaps();

		stbi_image_free(data);

		return texture;
	}
	else
	{
		stbi_uc* data = stbi_load(path.c_str(), &x, &y, &n, 0);
		if (!data)
		{
			Error("Texture File not find: " + path);
			return nullptr;
		}

		GLenum internalFormat, format;

		if (n == 1)
		{
			internalFormat = GL_R8;
			format = GL_RED;
		}
		else
		{
			if (srgb)
			{
				if (n == 4)
				{
					internalFormat = GL_SRGB8_ALPHA8;
					format = GL_RGBA;
				}
				else
				{
					internalFormat = GL_SRGB8;
					format = GL_RGB;
				}
			}
			else
			{
				if (n == 4)
				{
					internalFormat = GL_RGBA8;
					format = GL_RGBA;
				}
				else
				{
					internalFormat = GL_RGB8;
					format = GL_RGB;
				}
			}
		}

		auto texture = Texture2D::Create(x, y, 1, -1, 1, internalFormat, format, GL_UNSIGNED_BYTE);
		texture->WriteData(0, 0, data);
		texture->GenerateMipmaps();

		stbi_image_free(data);

		return texture;
	}
}
//=============================================================================
void Texture2D::WriteData(int arrayIndex, int mipLevel, void* data)
{
	if (m_numSamples > 1)
		Error("OPENGL: Multisampled texture data can only be assigned through Shaders or FBOs");
	else
	{
		int width = m_width;
		int height = m_height;

		for (int i = 0; i < mipLevel; i++)
		{
			width = std::max(1, width / 2);
			height = std::max(1, (height / 2));
		}

		WriteSubData(arrayIndex, mipLevel, 0, 0, width, height, data);
	}
}
//=============================================================================
void Texture2D::WriteSubData(int arrayIndex, int mipLevel, int xOffset, int yOffset, int width, int height, void* data)
{
	if (m_arraySize > 1)
		glTextureSubImage3D(m_glTex, mipLevel, xOffset, yOffset, arrayIndex, width, height, 1, m_format, m_type, data);
	else
		glTextureSubImage2D(m_glTex, mipLevel, xOffset, yOffset, width, height, m_format, m_type, data);
}
//=============================================================================
void Texture2D::WriteCompressedData(int arrayIndex, int mipLevel, size_t size, void* data)
{
	if (m_numSamples > 1)
		Error("OPENGL: Multisampled texture data can only be assigned through Shaders or FBOs");
	else
	{
		int width = m_width;
		int height = m_height;

		for (int i = 0; i < mipLevel; i++)
		{
			width = std::max(1, width / 2);
			height = std::max(1, (height / 2));
		}

		WriteCompressedSubData(arrayIndex, mipLevel, 0, 0, width, height, size, data);
	}
}
//=============================================================================
void Texture2D::WriteCompressedSubData(int arrayIndex, int mipLevel, int xOffset, int yOffset, int width, int height, size_t size, void* data)
{
	if (m_arraySize > 1)
		glCompressedTextureSubImage3D(m_glTex, mipLevel, xOffset, yOffset, arrayIndex, width, height, 1, m_internalFormat, size, data);
	else
		glCompressedTextureSubImage2D(m_glTex, mipLevel, xOffset, yOffset, width, height, m_internalFormat, size, data);
}
//=============================================================================
void Texture2D::ReadData(int mipLevel, std::vector<uint8_t>& buffer)
{
	int w, h;
	Extents(mipLevel, w, h);

	size_t size = IsCompressed(mipLevel) ? CompressedSize(mipLevel) : w * h * m_arraySize * PixelSizeFromType(m_type) * NumChannelsFromInternalFormat(m_format);
	buffer.resize(size);

	if (IsCompressed(mipLevel))
		glGetCompressedTextureImage(m_glTex, mipLevel, size, &buffer[0]);
	else
		glGetTextureImage(m_glTex, mipLevel, m_format, m_type, size, &buffer[0]);
}
//=============================================================================
void Texture2D::Extents(int mipLevel, int& width, int& height)
{
	glGetTextureLevelParameteriv(m_glTex, mipLevel, GL_TEXTURE_WIDTH, &width);
	glGetTextureLevelParameteriv(m_glTex, mipLevel, GL_TEXTURE_HEIGHT, &height);
}
//=============================================================================
void Texture2D::Resize(uint32_t w, uint32_t h)
{
	if (m_glTex != UINT32_MAX)
		glDeleteTextures(1, &m_glTex);

	m_version++;
	m_width = w;
	m_height = h;

	// Check if the max number of mip-levels possible with the current size is less than the earlier number, if so use the smaller number.
	int mipLevels = 1;

	int width = m_width;
	int height = m_height;

	while (width > 1 || height > 1)
	{
		width = std::max(1, (width / 2));
		height = std::max(1, (height / 2));
		mipLevels++;
	}

	if (mipLevels < m_mipLevels)
		m_mipLevels = mipLevels;

	allocate();
}
//=============================================================================
uint32_t Texture2D::Width() const
{
	return m_width;
}
//=============================================================================
uint32_t Texture2D::Height() const
{
	return m_height;
}
//=============================================================================
uint32_t Texture2D::NumSamples() const
{
	return m_numSamples;
}
//=============================================================================
void Texture2D::allocate()
{
	// Allocate memory for mip levels.
	if (m_arraySize > 1)
	{
		if (m_numSamples > 1)
			m_target = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
		else
			m_target = GL_TEXTURE_2D_ARRAY;

		glCreateTextures(m_target, 1, &m_glTex);

		if (m_numSamples > 1)
		{
			if (m_mipLevels > 1)
				Warning("OPENGL: Multisampled textures cannot have mipmaps. Setting mip levels to 1...");

			m_mipLevels = 1;
			glTextureStorage3DMultisample(m_glTex, m_numSamples, m_internalFormat, m_width, m_height, m_arraySize, true);
		}
		else
			glTextureStorage3D(m_glTex, m_mipLevels, m_internalFormat, m_width, m_height, m_arraySize);
	}
	else
	{
		if (m_numSamples > 1)
			m_target = GL_TEXTURE_2D_MULTISAMPLE;
		else
			m_target = GL_TEXTURE_2D;

		glCreateTextures(m_target, 1, &m_glTex);

		if (m_numSamples > 1)
		{
			if (m_mipLevels > 1)
				Warning("OPENGL: Multisampled textures cannot have mipmaps. Setting mip levels to 1...");

			m_mipLevels = 1;
			glTextureStorage2DMultisample(m_glTex, m_numSamples, m_internalFormat, m_width, m_height, true);
		}
		else
			glTextureStorage2D(m_glTex, m_mipLevels, m_internalFormat, m_width, m_height);
	}
}
//=============================================================================
Texture3D::Texture3D(uint32_t w, uint32_t h, uint32_t d, int mipLevels, GLenum internalFormat, GLenum format, GLenum type)
{
	m_internalFormat = internalFormat;
	m_arraySize = 1;
	m_format = format;
	m_type = type;
	m_width = w;
	m_height = h;
	m_depth = d;

	// If mip levels is -1, calculate mip levels
	if (mipLevels == -1)
	{
		m_mipLevels = 1;

		int width = m_width;
		int height = m_height;
		int depth = m_depth;

		while (width > 1 && height > 1 && depth > 1)
		{
			width = std::max(1, (width / 2));
			height = std::max(1, (height / 2));
			depth = std::max(1, (depth / 2));
			m_mipLevels++;
		}
	}
	else
		m_mipLevels = mipLevels;

	// Allocate memory for mip levels.
	m_target = GL_TEXTURE_3D;

	allocate();

	// Default sampling options.
	SetWrapping(GL_REPEAT, GL_REPEAT, GL_REPEAT);
	SetMagFilter(GL_LINEAR);

	if (m_mipLevels > 1)
		SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	else
		SetMinFilter(GL_LINEAR);
}
//=============================================================================
Texture3D::~Texture3D()
{
	closeTextureBase();
}
//=============================================================================
Texture3D::Ptr Texture3D::Create(uint32_t w, uint32_t h, uint32_t d, int mipLevels, GLenum internalFormat, GLenum format, GLenum type)
{
	return std::shared_ptr<Texture3D>(new Texture3D(w, h, d, mipLevels, internalFormat, format, type));
}
//=============================================================================
void Texture3D::WriteData(int slice, int mipLevel, void* data)
{
	int width = m_width;
	int height = m_height;

	for (int i = 0; i < mipLevel; i++)
	{
		width = std::max(1, width / 2);
		height = std::max(1, (height / 2));
	}

	WriteSubData(slice, mipLevel, 0, 0, width, height, data);
}
//=============================================================================
void Texture3D::WriteSubData(int slice, int mipLevel, int xOffset, int yOffset, int width, int height, void* data)
{
	glTextureSubImage3D(m_glTex, mipLevel, xOffset, yOffset, slice, width, height, 1, m_format, m_type, data);
}
//=============================================================================
void Texture3D::WriteCompressedData(int slice, int mipLevel, size_t size, void* data)
{
	int width = m_width;
	int height = m_height;

	for (int i = 0; i < mipLevel; i++)
	{
		width = std::max(1, width / 2);
		height = std::max(1, (height / 2));
	}

	WriteCompressedSubData(slice, mipLevel, 0, 0, width, height, size, data);
}
//=============================================================================
void Texture3D::WriteCompressedSubData(int slice, int mipLevel, int xOffset, int yOffset, int width, int height, size_t size, void* data)
{
	glCompressedTextureSubImage3D(m_glTex, mipLevel, xOffset, yOffset, slice, width, height, 1, m_internalFormat, size, data);
}
//=============================================================================
void Texture3D::ReadData(int mipLevel, std::vector<uint8_t>& buffer)
{
	int w, h, d;
	Extents(mipLevel, w, h, d);

	size_t size = IsCompressed(mipLevel) ? CompressedSize(mipLevel) : w * h * d * PixelSizeFromType(m_type) * NumChannelsFromInternalFormat(m_format);
	buffer.resize(size);

	if (IsCompressed(mipLevel))
		glGetCompressedTextureImage(m_glTex, mipLevel, size, &buffer[0]);
	else
		glGetTextureImage(m_glTex, mipLevel, m_format, m_type, size, &buffer[0]);
}
//=============================================================================
void Texture3D::Extents(int mipLevel, int& width, int& height, int& depth)
{
	glGetTextureLevelParameteriv(m_glTex, mipLevel, GL_TEXTURE_WIDTH, &width);
	glGetTextureLevelParameteriv(m_glTex, mipLevel, GL_TEXTURE_HEIGHT, &height);
	glGetTextureLevelParameteriv(m_glTex, mipLevel, GL_TEXTURE_DEPTH, &depth);
}
//=============================================================================
void Texture3D::Resize(uint32_t w, uint32_t h, uint32_t d)
{
	if (m_glTex != UINT32_MAX)
		glDeleteTextures(1, &m_glTex);

	m_version++;
	m_width = w;
	m_height = h;
	m_depth = d;

	// Check if the max number of mip-levels possible with the current size is less than the earlier number, if so use the smaller number.
	int mipLevels = 1;

	int width = m_width;
	int height = m_height;
	int depth = m_depth;

	while (width > 1 || height > 1 || depth > 1)
	{
		width = std::max(1, (width / 2));
		height = std::max(1, (height / 2));
		depth = std::max(1, (depth / 2));
		mipLevels++;
	}

	if (mipLevels < m_mipLevels)
		m_mipLevels = mipLevels;

	allocate();
}
//=============================================================================
uint32_t Texture3D::Width() const
{
	return m_width;
}
//=============================================================================
uint32_t Texture3D::Height() const
{
	return m_height;
}
//=============================================================================
uint32_t Texture3D::Depth() const
{
	return m_depth;
}
//=============================================================================
void Texture3D::allocate()
{
	glCreateTextures(m_target, 1, &m_glTex);
	glTextureStorage3D(m_glTex, m_mipLevels, m_internalFormat, m_width, m_height, m_depth);
}
//=============================================================================
TextureCube::TextureCube(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type)
{
	m_arraySize = arraySize;
	m_internalFormat = internalFormat;
	m_format = format;
	m_type = type;
	m_width = w;
	m_height = h;

	// If mip levels is -1, calculate mip levels
	if (mipLevels == -1)
	{
		m_mipLevels = 1;

		int width = m_width;
		int height = m_height;

		while (width > 1 && height > 1)
		{
			width = std::max(1, (width / 2));
			height = std::max(1, (height / 2));
			m_mipLevels++;
		}
	}
	else
		m_mipLevels = mipLevels;

	// Allocate memory for mip levels.
	if (arraySize > 1)
		m_target = GL_TEXTURE_CUBE_MAP_ARRAY;
	else
		m_target = GL_TEXTURE_CUBE_MAP;

	allocate();

	// Default sampling options.
	SetWrapping(GL_REPEAT, GL_REPEAT, GL_REPEAT);
	SetMagFilter(GL_LINEAR);

	if (m_mipLevels > 1)
		SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	else
		SetMinFilter(GL_LINEAR);
}
//=============================================================================
TextureCube::~TextureCube()
{
	closeTextureBase();
}
//=============================================================================
TextureCube::Ptr TextureCube::Create(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type)
{
	return std::shared_ptr<TextureCube>(new TextureCube(w, h, arraySize, mipLevels, internalFormat, format, type));
}
//=============================================================================
TextureCube::Ptr TextureCube::CreateFromFiles(std::string path[], bool srgb)
{
	if (GetFileExtension(path[0]) == "hdr")
	{
		// Load the first image to determine format and dimensions.
		std::string texPath = path[0];

		int    x, y, n;
		float* data = stbi_loadf(texPath.c_str(), &x, &y, &n, 3);
		if (!data)
		{
			Error("Texture File not find: " + texPath);
			return nullptr;
		}

		GLenum internalFormat, format;

		internalFormat = GL_RGB32F;
		format = GL_RGB;

		auto cube = TextureCube::Create(x, y, 1, -1, internalFormat, format, GL_FLOAT);
		cube->WriteData(0, 0, 0, data);
		stbi_image_free(data);

		for (int i = 1; i < 6; i++)
		{
			texPath = path[i];
			data = stbi_loadf(texPath.c_str(), &x, &y, &n, 3);
			if (!data)
			{
				Error("Texture File not find: " + texPath);
				return nullptr;
			}

			cube->WriteData(i, 0, 0, data);
			stbi_image_free(data);
		}

		return cube;
	}
	else
	{
		// Load the first image to determine format and dimensions.
		std::string texPath = path[0];

		int      x, y, n;
		stbi_uc* data = stbi_load(texPath.c_str(), &x, &y, &n, 3);
		if (!data)
		{
			Error("Texture File not find: " + texPath);
			return nullptr;
		}

		GLenum internalFormat, format;

		if (srgb)
		{
			internalFormat = GL_SRGB8;
			format = GL_RGB;
		}
		else
		{
			internalFormat = GL_RGBA8;
			format = GL_RGB;
		}

		auto cube = TextureCube::Create(x, y, 1, -1, internalFormat, format, GL_UNSIGNED_BYTE);

		cube->WriteData(0, 0, 0, data);
		stbi_image_free(data);

		for (int i = 1; i < 6; i++)
		{
			texPath = path[i];
			data = stbi_load(texPath.c_str(), &x, &y, &n, 3);
			if (!data)
			{
				Error("Texture File not find: " + texPath);
				return nullptr;
			}

			cube->WriteData(i, 0, 0, data);
			stbi_image_free(data);
		}

		return cube;
	}
}
//=============================================================================
void TextureCube::WriteData(int faceIndex, int arrayIndex, int mipLevel, void* data)
{
	int width = m_width;
	int height = m_height;

	for (int i = 0; i < mipLevel; i++)
	{
		width = std::max(1, (width / 2));
		height = std::max(1, (height / 2));
	}

	if (m_arraySize == 1)
		arrayIndex = 0;

	WriteSubData(faceIndex, arrayIndex, mipLevel, 0, 0, width, height, data);
}
//=============================================================================
void TextureCube::WriteSubData(int faceIndex, int arrayIndex, int mipLevel, int xOffset, int yOffset, int width, int height, void* data)
{
	glTextureSubImage3D(m_glTex, mipLevel, xOffset, yOffset, arrayIndex * 6 + faceIndex, width, height, 1, m_format, m_type, data);
}
//=============================================================================
void TextureCube::WriteCompressedData(int faceIndex, int arrayIndex, int mipLevel, size_t size, void* data)
{
	int width = m_width;
	int height = m_height;

	for (int i = 0; i < mipLevel; i++)
	{
		width = std::max(1, (width / 2));
		height = std::max(1, (height / 2));
	}

	if (m_arraySize == 1)
		arrayIndex = 0;

	WriteCompressedSubData(faceIndex, arrayIndex, mipLevel, 0, 0, width, height, size, data);
}
//=============================================================================
void TextureCube::WriteCompressedSubData(int faceIndex, int arrayIndex, int mipLevel, int xOffset, int yOffset, int width, int height, size_t size, void* data)
{
	glCompressedTextureSubImage3D(m_glTex, mipLevel, xOffset, yOffset, arrayIndex * 6 + faceIndex, width, height, m_internalFormat, size, m_type, data);
}
//=============================================================================
void TextureCube::ReadData(int mipLevel, std::vector<uint8_t>& buffer)
{
	int w, h;
	Extents(mipLevel, w, h);

	size_t size = IsCompressed(mipLevel) ? CompressedSize(mipLevel) : w * h * 6 * m_arraySize * PixelSizeFromType(m_type) * NumChannelsFromInternalFormat(m_format);
	buffer.resize(size);

	if (IsCompressed(mipLevel))
		glGetCompressedTextureImage(m_glTex, mipLevel, size, &buffer[0]);
	else
		glGetTextureImage(m_glTex, mipLevel, m_format, m_type, size, &buffer[0]);
}
//=============================================================================
void TextureCube::Extents(int mipLevel, int& width, int& height)
{
	glGetTextureLevelParameteriv(m_glTex, mipLevel, GL_TEXTURE_WIDTH, &width);
	glGetTextureLevelParameteriv(m_glTex, mipLevel, GL_TEXTURE_HEIGHT, &height);
}
//=============================================================================
void TextureCube::Resize(uint32_t w, uint32_t h)
{
	if (m_glTex != UINT32_MAX)
		glDeleteTextures(1, &m_glTex);

	glCreateTextures(m_target, 1, &m_glTex);

	m_version++;
	m_width = w;
	m_height = h;

	// Check if the max number of mip-levels possible with the current size is less than the earlier number, if so use the smaller number.
	int mipLevels = 1;

	int width = m_width;
	int height = m_height;

	while (width > 1 || height > 1)
	{
		width = std::max(1, (width / 2));
		height = std::max(1, (height / 2));
		mipLevels++;
	}

	if (mipLevels < m_mipLevels)
		m_mipLevels = mipLevels;

	allocate();
}
//=============================================================================
uint32_t TextureCube::Width() const
{
	return m_width;
}
//=============================================================================
uint32_t TextureCube::Height() const
{
	return m_height;
}
//=============================================================================
void TextureCube::allocate()
{
	glCreateTextures(m_target, 1, &m_glTex);

	if (m_arraySize > 1)
		glTextureStorage3D(m_glTex, m_mipLevels, m_internalFormat, m_width, m_height, m_arraySize);
	else
		glTextureStorage2D(m_glTex, m_mipLevels, m_internalFormat, m_width, m_height);
}
//=============================================================================
Texture1DView::Texture1DView(Texture1D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers)
	: Texture1D()
{
	m_target = newTarget;
	m_arraySize = numLayers;
	m_internalFormat = originTexture->InternalFormat();
	m_format = originTexture->Format();
	m_type = originTexture->Type();
	m_width = originTexture->Width();

	glGenTextures(1, &m_glTex);
	glTextureView(m_glTex, m_target, originTexture->Id(), m_internalFormat, minLevel, numLevels, minLayer, numLayers);
}
//=============================================================================
Texture1DView::~Texture1DView()
{
	closeTextureBase();
}
//=============================================================================
Texture1DView::Ptr Texture1DView::Create(Texture1D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers)
{
	return std::shared_ptr<Texture1DView>(new Texture1DView(originTexture, newTarget, minLevel, numLevels, minLayer, numLayers));
}
//=============================================================================
Texture2DView::Texture2DView(Texture2D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers)
	: Texture2D()
{
	m_target = newTarget;
	m_mipLevels = numLevels;
	m_arraySize = numLayers;
	m_internalFormat = originTexture->InternalFormat();
	m_format = originTexture->Format();
	m_type = originTexture->Type();
	m_width = originTexture->Width();
	m_height = originTexture->Height();

	glGenTextures(1, &m_glTex);
	glTextureView(m_glTex, m_target, originTexture->Id(), m_internalFormat, minLevel, numLevels, minLayer, numLayers);
}
//=============================================================================
Texture2DView::~Texture2DView()
{
	closeTextureBase();
}
//=============================================================================
Texture2DView::Ptr Texture2DView::Create(Texture2D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers)
{
	return std::shared_ptr<Texture2DView>(new Texture2DView(originTexture, newTarget, minLevel, numLevels, minLayer, numLayers));
}
//=============================================================================
Texture3DView::Texture3DView(Texture3D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers)
	: Texture3D()
{
	m_target = newTarget;
	m_mipLevels = numLevels;
	m_arraySize = numLayers;
	m_internalFormat = originTexture->InternalFormat();
	m_format = originTexture->Format();
	m_type = originTexture->Type();
	m_width = originTexture->Width();
	m_height = originTexture->Height();
	m_depth = originTexture->Depth();

	glGenTextures(1, &m_glTex);
	glTextureView(m_glTex, m_target, originTexture->Id(), m_internalFormat, minLevel, numLevels, minLayer, numLayers);
}
//=============================================================================
Texture3DView::~Texture3DView()
{
	closeTextureBase();
}
//=============================================================================
Texture3DView::Ptr Texture3DView::Create(Texture3D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers)
{
	return std::shared_ptr<Texture3DView>(new Texture3DView(originTexture, newTarget, minLevel, numLevels, minLayer, numLayers));
}
//=============================================================================
TextureCubeView::TextureCubeView(TextureCube::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers)
	: TextureCube()
{
	m_target = newTarget;
	m_mipLevels = numLevels;
	m_arraySize = numLayers;
	m_internalFormat = originTexture->InternalFormat();
	m_format = originTexture->Format();
	m_type = originTexture->Type();
	m_width = originTexture->Width();
	m_height = originTexture->Height();

	glGenTextures(1, &m_glTex);
	glTextureView(m_glTex, m_target, originTexture->Id(), m_internalFormat, minLevel, numLevels, minLayer, numLayers);
}
//=============================================================================
TextureCubeView::~TextureCubeView()
{
	closeTextureBase();
}
//=============================================================================
TextureCubeView::Ptr TextureCubeView::Create(TextureCube::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers)
{
	return std::shared_ptr<TextureCubeView>(new TextureCubeView(originTexture, newTarget, minLevel, numLevels, minLayer, numLayers));
}
//=============================================================================