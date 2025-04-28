#pragma once

class BaseTexture
{
protected:
	BaseTexture() {}

	GLenum m_textureTarget = 0;
	GLuint m_textureObj = 0;
};

class Texture : public BaseTexture
{
public:
	Texture(GLenum TextureTarget, const std::string& FileName);

	Texture(GLenum TextureTarget);

	// Should be called once to load the texture
	bool Load();

	void Load(const std::string& Filename);

	void Load(unsigned int BufferSize, void* pImageData);

	void LoadRaw(int Width, int Height, int BPP, const unsigned char* pImageData);

	void LoadF32(int Width, int Height, const float* pImageData);

	// Must be called at least once for the specific texture unit
	void Bind(GLenum TextureUnit);

	void GetImageSize(int& ImageWidth, int& ImageHeight)
	{
		ImageWidth = m_imageWidth;
		ImageHeight = m_imageHeight;
	}

	GLuint GetTexture() const { return m_textureObj; }

	GLuint64 GetBindlessHandle() const { return m_bindlessHandle; }

private:
	void LoadInternal(const void* pImageData);
	void LoadInternalNonDSA(const void* pImageData);
	void LoadInternalDSA(const void* pImageData);

	void BindInternalNonDSA(GLenum TextureUnit);
	void BindInternalDSA(GLenum TextureUnit);

	bool m_isKTX = false;
	gli::gl::format m_ktxFormat;

	std::string m_fileName;
	GLuint64 m_bindlessHandle = -1;
	int m_imageWidth = 0;
	int m_imageHeight = 0;
	int m_imageBPP = 0;
};