#include "stdafx.h"
#include "RHIFramebuffer.h"
#include "Core.h"
//=============================================================================
RenderBuffer::RenderBuffer(GLenum internalFormat, GLsizei width, GLsizei height)
	: m_internalFormat(internalFormat)
	, m_width(width)
	, m_height(height)
{
	glCreateRenderbuffers(1, &m_glRBO);
	glNamedRenderbufferStorage(m_glRBO, internalFormat, width, height);
}
//=============================================================================
RenderBuffer::RenderBuffer(GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height)
	: m_internalFormat(internalFormat)
	, m_width(width)
	, m_height(height)
{
	glCreateRenderbuffers(1, &m_glRBO);
	glNamedRenderbufferStorageMultisample(m_glRBO, samples, internalFormat, width, height);
}
//=============================================================================
RenderBuffer::~RenderBuffer()
{
	glDeleteRenderbuffers(1, &m_glRBO);
}
//=============================================================================
RenderBuffer::Ptr RenderBuffer::Create(GLenum internalFormat, GLsizei width, GLsizei height)
{
	return std::shared_ptr<RenderBuffer>(new RenderBuffer(internalFormat, width, height));
}
//=============================================================================
RenderBuffer::Ptr RenderBuffer::CreateMultisample(GLsizei samples, GLenum internalFormat, GLsizei width, GLsizei height)
{
	return std::shared_ptr<RenderBuffer>(new RenderBuffer(samples, internalFormat, width, height));
}
//=============================================================================
GLuint RenderBuffer::Id() const
{
	return m_glRBO;
}
//=============================================================================
Framebuffer::Framebuffer(std::vector<Texture2D::Ptr> colorAttachments, Texture2D::Ptr depthStencilAttachment)
{
	glCreateFramebuffers(1, &m_glFBO);

	GLuint attachments[16];

	for (int i = 0; i < colorAttachments.size(); i++)
	{
		glNamedFramebufferTexture(m_glFBO, GL_COLOR_ATTACHMENT0 + i, colorAttachments[i]->Id(), 0);
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	glNamedFramebufferDrawBuffers(m_glFBO, colorAttachments.size(), attachments);

	if (depthStencilAttachment)
		glNamedFramebufferTexture(m_glFBO, GL_DEPTH_ATTACHMENT, depthStencilAttachment->Id(), 0);

	checkStatus();
}
//=============================================================================
Framebuffer::Framebuffer(std::vector<Texture2D::Ptr> colorAttachments, RenderBuffer::Ptr depthStencilAttachment)
{
	glCreateFramebuffers(1, &m_glFBO);

	GLuint attachments[16];

	for (int i = 0; i < colorAttachments.size(); i++)
	{
		glNamedFramebufferTexture(m_glFBO, GL_COLOR_ATTACHMENT0 + i, colorAttachments[i]->Id(), 0);
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	glNamedFramebufferDrawBuffers(m_glFBO, colorAttachments.size(), attachments);

	if (depthStencilAttachment)
		glNamedFramebufferRenderbuffer(m_glFBO, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthStencilAttachment->Id());

	checkStatus();
}
//=============================================================================
Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_glFBO);
}
//=============================================================================
Framebuffer::Ptr Framebuffer::Create(std::vector<Texture2D::Ptr> colorAttachments, Texture2D::Ptr depthStencilAttachment)
{
	return std::shared_ptr<Framebuffer>(new Framebuffer(colorAttachments, depthStencilAttachment));
}
//=============================================================================
Framebuffer::Ptr Framebuffer::Create(std::vector<Texture2D::Ptr> colorAttachments, RenderBuffer::Ptr depthStencilAttachment)
{
	return std::shared_ptr<Framebuffer>(new Framebuffer(colorAttachments, depthStencilAttachment));
}
//=============================================================================
void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_glFBO);
}
//=============================================================================
void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//=============================================================================
void Framebuffer::SetDrawBuffer(GLenum buffer)
{
	glNamedFramebufferDrawBuffer(m_glFBO, buffer);
}
//=============================================================================
void Framebuffer::SetDrawBuffers(GLsizei size, const GLenum* buffers)
{
	glNamedFramebufferDrawBuffers(m_glFBO, size, buffers);
}
//=============================================================================
void Framebuffer::SetName(const std::string& name)
{
	glObjectLabel(GL_FRAMEBUFFER, m_glFBO, name.size(), name.c_str());
}
//=============================================================================
void Framebuffer::checkStatus()
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::string error = "Framebuffer Incomplete: ";

		switch (status)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		{
			error += "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		{
			error += "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		{
			error += "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		{
			error += "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			break;
		}
		case GL_FRAMEBUFFER_UNSUPPORTED:
		{
			error += "GL_FRAMEBUFFER_UNSUPPORTED";
			break;
		}
		default:
			break;
		}

		Error(error);
	}
}
//=============================================================================