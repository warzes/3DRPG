#include "stdafx.h"
#include "RHIFramebuffer.h"
#include "Core.h"
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
void Framebuffer::SetName(const std::string& name)
{
	setName(m_glFBO, name);
}
//=============================================================================
void Framebuffer::setName(const GLuint& name, const std::string& label)
{
	m_name = label;
	glObjectLabel(m_identifier, name, label.size(), label.c_str());
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