#pragma once

#include "RHITexture.h"

class Framebuffer final
{
public:
	~Framebuffer();

	using Ptr = std::shared_ptr<Framebuffer>;

	static Framebuffer::Ptr Create(std::vector<Texture2D::Ptr> colorAttachments, Texture2D::Ptr depthStencilAttachment);

	void Bind();
	void Unbind();

	void SetName(const std::string& name);

private:
	Framebuffer(std::vector<Texture2D::Ptr> colorAttachments, Texture2D::Ptr depthStencilAttachment);

	void setName(const GLuint& name, const std::string& label);
	void checkStatus();

	GLuint       m_glFBO;
	const GLenum m_identifier{ GL_FRAMEBUFFER };
	std::string  m_name;
};