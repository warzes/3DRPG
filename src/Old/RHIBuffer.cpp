#include "stdafx.h"
#include "RHIBuffer.h"
//=============================================================================
Buffer::Buffer(GLenum type, GLenum flags, size_t size, void* data)
	: m_type(type)
	, m_size(size)
{
	glCreateBuffers(1, &m_glBuffer);
	glNamedBufferStorage(m_glBuffer, size, data, flags);
}
//=============================================================================
Buffer::~Buffer()
{
	glDeleteBuffers(1, &m_glBuffer);
}
//=============================================================================
Buffer::Ptr Buffer::Create(GLenum type, GLenum flags, size_t size, void* data)
{
	return std::shared_ptr<Buffer>(new Buffer(type, flags, size, data));
}
//=============================================================================
void Buffer::Bind()
{
	glBindBuffer(m_type, m_glBuffer);
}
//=============================================================================
void Buffer::Bind(GLenum type)
{
	glBindBuffer(type, m_glBuffer);
}
//=============================================================================
void Buffer::BindBase(int index)
{
	glBindBufferBase(m_type, index, m_glBuffer);
}
//=============================================================================
void Buffer::BindRange(int index, size_t offset, size_t size)
{
	glBindBufferRange(m_type, index, m_glBuffer, offset, size);
}
//=============================================================================
void Buffer::BindBase(GLenum type, int index)
{
	glBindBufferBase(type, index, m_glBuffer);
}
//=============================================================================
void Buffer::BindRange(GLenum type, int index, size_t offset, size_t size)
{
	glBindBufferRange(type, index, m_glBuffer, offset, size);
}
//=============================================================================
void Buffer::Unbind()
{
	glBindBuffer(m_type, 0);
}
//=============================================================================
void* Buffer::Map(GLenum access)
{
	return glMapNamedBuffer(m_glBuffer, access);
}
//=============================================================================
void* Buffer::MapRange(GLenum access, size_t offset, size_t size)
{
	return glMapNamedBufferRange(m_glBuffer, offset, size, access);
}
//=============================================================================
void Buffer::Unmap()
{
	glUnmapNamedBuffer(m_glBuffer);
}
//=============================================================================
void Buffer::FlushMappedRange(size_t offset, size_t length)
{
	glFlushMappedNamedBufferRange(m_glBuffer, offset, length);
}
//=============================================================================
void Buffer::WriteData(size_t offset, size_t size, void* data)
{
	glNamedBufferSubData(m_glBuffer, offset, size, data);
}
//=============================================================================
void Buffer::Copy(Buffer::Ptr dst, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
	glCopyNamedBufferSubData(m_glBuffer, dst->m_glBuffer, readOffset, writeOffset, size);
}
//=============================================================================
size_t Buffer::Size() const
{
	return m_size;
}
//=============================================================================
void Buffer::SetName(const std::string& name)
{
	setName(m_glBuffer, name);
}
//=============================================================================
void Buffer::setName(const GLuint& name, const std::string& label)
{
	m_name = label;
	glObjectLabel(m_identifier, name, label.size(), label.c_str());
}
//=============================================================================