#pragma once

#include "RHIBuffer.h"

struct VertexAttrib final
{
	uint32_t numSubElements;
	uint32_t type;
	bool     normalized;
	uint32_t offset;
};

class VertexArray final
{
public:
	~VertexArray();

	using Ptr = std::shared_ptr<VertexArray>;

	static VertexArray::Ptr Create(Buffer::Ptr vbo, Buffer::Ptr ibo, size_t vertexSize, int attribCount, VertexAttrib attribs[]);

	void Bind();
	void Unbind();

	void SetName(const std::string& name);

private:
	VertexArray(Buffer::Ptr vbo, Buffer::Ptr ibo, size_t vertexSize, int attribCount, VertexAttrib attribs[]);

	void setName(const GLuint& name, const std::string& label);

	GLuint       m_glVAO;
	const GLenum m_identifier{ GL_VERTEX_ARRAY };
	std::string  m_name;
};