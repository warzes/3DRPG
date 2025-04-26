#pragma once

class TextureBase
{
public:
	// Bind texture to specified texture unit i.e GL_TEXTURE<unit>.
	void Bind(uint32_t unit);
	void Unbind(uint32_t unit); // TODO: в рендер

	// Binding to image units.
	void BindImage(uint32_t unit, uint32_t mip_level, uint32_t layer, GLenum access, GLenum format);

	// Mipmap generation.
	void GenerateMipmaps();

	// Getters.
	GLuint   Id() const;
	GLenum   Target() const;
	uint32_t ArraySize() const;
	uint32_t Version() const;
	uint32_t MipLevels() const;
	auto     InternalFormat() const { return m_internalFormat; }
	auto     Format() const { return m_format; }
	auto     Type() const { return m_type; }

	// Texture sampler functions.
	void     SetWrapping(GLenum s, GLenum t, GLenum r);
	void     SetBorderColor(float r, float g, float b, float a);
	void     SetMinFilter(GLenum filter);
	void     SetMagFilter(GLenum filter);
	void     SetCompareMode(GLenum mode);
	void     SetCompareFunc(GLenum func);
	bool     IsCompressed(int mipLevel);
	int      CompressedSize(int mipLevel);
	GLuint64 MakeTextureHandleResident();
	void     MakeTextureHandleNonResident();
	GLuint64 MakeImageHandleResident(GLenum access, GLint level, GLboolean layered, GLint layer);
	void     MakeImageHandleNonResident();

	void     SetName(const std::string& name);

protected:
	void closeTextureBase();
	void setName(const GLuint& name, const std::string& label);

	GLuint      m_glTex{ UINT32_MAX };
	GLenum      m_target{ 0 };
	GLenum      m_internalFormat{ 0 };
	GLenum      m_format{ 0 };
	GLenum      m_type{ 0 };
	uint32_t    m_version{ 0 };
	uint32_t    m_arraySize{ 0 };
	uint32_t    m_mipLevels{ 0 };
	GLuint64    m_textureHandle{ 0 };
	GLuint64    m_imageHandle{ 0 };

private:
	const GLenum m_identifier{ GL_TEXTURE };
	std::string m_name;
};

class Texture1D : public TextureBase
{
public:
	~Texture1D();

	using Ptr = std::shared_ptr<Texture1D>;

	static Texture1D::Ptr Create(uint32_t w, uint32_t arraySize, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type);

	void     WriteData(int arrayIndex, int mipLevel, void* data);
	void     WriteSubData(int arrayIndex, int mipLevel, int xOffset, int width, void* data);
	void     WriteCompressedData(int arrayIndex, int mipLevel, int size, void* data);
	void     WriteCompressedSubData(int arrayIndex, int mipLevel, int xOffset, int width, int size, void* data);
	void     Resize(uint32_t w);
	uint32_t Width() const;

protected:
	Texture1D() = default;

	uint32_t m_width;

private:
	Texture1D(uint32_t w, uint32_t arraySize, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type);

	void allocate();
};

class Texture2D : public TextureBase
{
public:
	~Texture2D();

	using Ptr = std::shared_ptr<Texture2D>;

	static Texture2D::Ptr Create(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type);
	static Texture2D::Ptr CreateFromFile(std::string path, bool flipVertical = true, bool srgb = false);

	void     WriteData(int arrayIndex, int mipLevel, void* data);
	void     WriteSubData(int arrayIndex, int mipLevel, int xOffset, int yOffset, int width, int height, void* data);
	void     WriteCompressedData(int arrayIndex, int mipLevel, size_t size, void* data);
	void     WriteCompressedSubData(int arrayIndex, int mipLevel, int xOffset, int yOffset, int width, int height, size_t size, void* data);
	void     ReadData(int mipLevel, std::vector<uint8_t>& buffer);
	void     Extents(int mipLevel, int& width, int& height);
	void     Resize(uint32_t w, uint32_t h);
	uint32_t Width() const;
	uint32_t Height() const;
	uint32_t NumSamples() const;

protected:
	Texture2D() = default;

	uint32_t m_width;
	uint32_t m_height;

private:
	Texture2D(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, uint32_t numSamples, GLenum internalFormat, GLenum format, GLenum type);
	void allocate();


	uint32_t m_numSamples;
};

class Texture3D : public TextureBase
{
public:
	~Texture3D();

	using Ptr = std::shared_ptr<Texture3D>;

	static Texture3D::Ptr Create(uint32_t w, uint32_t h, uint32_t d, int mipLevels, GLenum internalFormat, GLenum format, GLenum type);

	void     WriteData(int slice, int mipLevel, void* data);
	void     WriteSubData(int slice, int mipLevel, int xOffset, int yOffset, int width, int height, void* data);
	void     WriteCompressedData(int slice, int mipLevel, size_t size, void* data);
	void     WriteCompressedSubData(int slice, int mipLevel, int xOffset, int yOffset, int width, int height, size_t size, void* data);
	void     ReadData(int mipLevel, std::vector<uint8_t>& buffer);
	void     Extents(int mipLevel, int& width, int& height, int& depth);
	void     Resize(uint32_t w, uint32_t h, uint32_t d);
	uint32_t Width() const;
	uint32_t Height() const;
	uint32_t Depth() const;

protected:
	Texture3D() = default;

	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_depth;

private:
	Texture3D(uint32_t w, uint32_t h, uint32_t d, int mipLevels, GLenum internalFormat, GLenum format, GLenum type);

	void allocate();
};

class TextureCube : public TextureBase
{
public:
	~TextureCube();

	using Ptr = std::shared_ptr<TextureCube>;

	static TextureCube::Ptr Create(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type);
	static TextureCube::Ptr CreateFromFiles(std::string path[], bool srgb);

	void     WriteData(int faceIndex, int arrayIndex, int mipLevel, void* data);
	void     WriteSubData(int faceIndex, int arrayIndex, int mipLevel, int xOffset, int yOffset, int width, int height, void* data);
	void     WriteCompressedData(int faceIndex, int arrayIndex, int mipLevel, size_t size, void* data);
	void     WriteCompressedSubData(int faceIndex, int arrayIndex, int mipLevel, int xOffset, int yOffset, int width, int height, size_t size, void* data);
	void     ReadData(int mipLevel, std::vector<uint8_t>& buffer);
	void     Extents(int mipLevel, int& width, int& height);
	void     Resize(uint32_t w, uint32_t h);
	uint32_t Width() const;
	uint32_t Height() const;

protected:
	TextureCube() = default;

	uint32_t m_width;
	uint32_t m_height;

private:
	TextureCube(uint32_t w, uint32_t h, uint32_t arraySize, int32_t mipLevels, GLenum internalFormat, GLenum format, GLenum type);

	void allocate();
};

class Texture1DView final : public Texture1D
{
public:
	~Texture1DView();

	using Ptr = std::shared_ptr<Texture1DView>;

	static Texture1DView::Ptr Create(Texture1D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers);

	auto OriginalTexture() { return m_originTexture.lock(); }

private:
	Texture1DView(Texture1D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers);

	std::weak_ptr<Texture1D> m_originTexture;
};

class Texture2DView final : public Texture2D
{
public:
	~Texture2DView();

	using Ptr = std::shared_ptr<Texture2DView>;

	static Texture2DView::Ptr Create(Texture2D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers);

	auto OriginalTexture() { return m_originTexture.lock(); }

private:
	Texture2DView(Texture2D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers);

	std::weak_ptr<Texture2D> m_originTexture;
};

class Texture3DView final : public Texture3D
{
public:
	~Texture3DView();

	using Ptr = std::shared_ptr<Texture3DView>;

	static Texture3DView::Ptr Create(Texture3D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers);

	auto OriginalTexture() { return m_originTexture.lock(); }

private:
	Texture3DView(Texture3D::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers);

	std::weak_ptr<Texture3D> m_originTexture;
};

class TextureCubeView final : public TextureCube
{
public:
	~TextureCubeView();

	using Ptr = std::shared_ptr<TextureCubeView>;

	static TextureCubeView::Ptr Create(TextureCube::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers);

	auto OriginalTexture() { return m_originTexture.lock(); }

private:
	TextureCubeView(TextureCube::Ptr originTexture, GLenum newTarget, int minLevel, int numLevels, int minLayer, int numLayers);

	std::weak_ptr<TextureCube> m_originTexture;
};