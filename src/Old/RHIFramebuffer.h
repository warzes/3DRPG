#pragma once

#include "RHITexture.h"

class RenderBuffer final
{
public:
	~RenderBuffer();

	using Ptr = std::shared_ptr<RenderBuffer>;

	static RenderBuffer::Ptr Create(GLenum internalFormat, GLsizei width, GLsizei height);
	static RenderBuffer::Ptr CreateMultisample(GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height);

	GLuint Id() const;

private:
	RenderBuffer(GLenum internalFormat, GLsizei width, GLsizei height);
	RenderBuffer(GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height);

	GLuint  m_glRBO{ 0 };
	GLenum  m_internalFormat{ 0 };
	GLsizei m_width{ 0 };
	GLsizei m_height{ 0 };
};

class Framebuffer final
{
public:
	~Framebuffer();

	using Ptr = std::shared_ptr<Framebuffer>;

	static Framebuffer::Ptr Create(std::vector<Texture2D::Ptr> colorAttachments, Texture2D::Ptr depthStencilAttachment);
	static Framebuffer::Ptr Create(std::vector<Texture2D::Ptr> colorAttachments, RenderBuffer::Ptr depthStencilAttachment);

	void Bind();
	void Unbind();

	/*
	позволяет рисовать только в конкретный объект фреймбуферs
	- GL_NONE - не рисовать в цвет
	- GL_FRONT, GL_BACK - системный фреймбуфер
	- GL_COLOR_ATTACHMENTi
	*/
	void SetDrawBuffer(GLenum buffer);
	void SetDrawBuffers(GLsizei size, const GLenum* buffers);

	void SetName(const std::string& name);

private:
	Framebuffer(std::vector<Texture2D::Ptr> colorAttachments, Texture2D::Ptr depthStencilAttachment);
	Framebuffer(std::vector<Texture2D::Ptr> colorAttachments, RenderBuffer::Ptr depthStencilAttachment);

	void checkStatus();

	GLuint m_glFBO{ 0 };
};