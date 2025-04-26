#pragma once

class Texture2DScopedBind final
{
public:
	Texture2DScopedBind(GLuint texture)
	{
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_lastTexture);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	~Texture2DScopedBind()
	{
		glBindTexture(GL_TEXTURE_2D, m_lastTexture);
	}

private:
	GLint m_lastTexture;
};