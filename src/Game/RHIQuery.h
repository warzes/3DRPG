#pragma once

class Query final
{
public:
	Query();
	~Query();
	void QueryCounter(GLenum type);
	void Begin(GLenum type);
	void End(GLenum type);
	void Result64(uint64_t* ptr);
	bool ResultAvailable();

	void SetName(const std::string& name);

private:
	void setName(const GLuint& name, const std::string& label);

	GLuint       m_query;
	const GLenum m_identifier{ GL_QUERY };
	std::string  m_name;
};