#pragma once

class Buffer final
{
public:
	~Buffer();

	using Ptr = std::shared_ptr<Buffer>;

	static Buffer::Ptr Create(GLenum type, GLenum flags, size_t size, void* data = nullptr);

	void   Bind();
	void   Bind(GLenum type);
	void   BindBase(int index);
	void   BindRange(int index, size_t offset, size_t size);
	void   BindBase(GLenum type, int index);
	void   BindRange(GLenum type, int index, size_t offset, size_t size);
	void   Unbind();
	void*  Map(GLenum access);
	void*  MapRange(GLenum access, size_t offset, size_t size);
	void   Unmap();
	void   FlushMappedRange(size_t offset, size_t length);
	void   WriteData(size_t offset, size_t size, void* data);
	void   Copy(Buffer::Ptr dst, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
	size_t Size() const;

	void   SetName(const std::string& name);

private:
	Buffer(GLenum type, GLenum flags, size_t size, void* data);

	void setName(const GLuint& name, const std::string& label);

	GLenum       m_type;
	GLuint       m_glBuffer;
	size_t       m_size;

	const GLenum m_identifier{ GL_BUFFER };
	std::string  m_name;
};