#include "stdafx.h"
#include "RHIVertexArray.h"
//=============================================================================
VertexArray::VertexArray(Buffer::Ptr vbo, Buffer::Ptr ibo, size_t vertexSize, int attribCount, VertexAttrib attribs[])
{
	glGenVertexArrays(1, &m_glVAO);
	glBindVertexArray(m_glVAO);

	vbo->Bind();
	if (ibo) ibo->Bind();

	for (uint32_t i = 0; i < attribCount; i++)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i,
			attribs[i].numSubElements,
			attribs[i].type,
			attribs[i].normalized,
			vertexSize,
			(GLvoid*)((uint64_t)attribs[i].offset));
	}

	glBindVertexArray(0);

	vbo->Unbind();

	if (ibo)
		ibo->Unbind();
}
//=============================================================================
VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_glVAO);
}
//=============================================================================
VertexArray::Ptr VertexArray::Create(Buffer::Ptr vbo, Buffer::Ptr ibo, size_t vertexSize, int attribCount, VertexAttrib attribs[])
{
	return std::shared_ptr<VertexArray>(new VertexArray(vbo, ibo, vertexSize, attribCount, attribs));
}
//=============================================================================
void VertexArray::Bind()
{
	glBindVertexArray(m_glVAO);
}
//=============================================================================
void VertexArray::Unbind()
{
	glBindVertexArray(0);
}
//=============================================================================
void VertexArray::SetName(const std::string& name)
{
	setName(m_glVAO, name);
}
//=============================================================================
void VertexArray::setName(const GLuint& name, const std::string& label)
{
	m_name = label;
	glObjectLabel(m_identifier, name, label.size(), label.c_str());
}
//=============================================================================