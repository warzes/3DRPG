#include "stdafx.h"
#include "RHIQuery.h"
//=============================================================================
Query::Query()
{
	glGenQueries(1, &m_query);
}
//=============================================================================
Query::~Query()
{
	glDeleteQueries(1, &m_query);
}
//=============================================================================
void Query::QueryCounter(GLenum type)
{
	glQueryCounter(m_query, type);
}
//=============================================================================
void Query::Begin(GLenum type)
{
	glBeginQuery(type, m_query);
}
//=============================================================================
void Query::End(GLenum type)
{
	glEndQuery(type);
}
//=============================================================================
void Query::Result64(uint64_t* ptr)
{
	glGetQueryObjectui64v(m_query, GL_QUERY_RESULT, ptr);
}
//=============================================================================
bool Query::ResultAvailable()
{
	int done = 0;
	glGetQueryObjectiv(m_query, GL_QUERY_RESULT_AVAILABLE, &done);
	return done == 1;
}
//=============================================================================
void Query::SetName(const std::string& name)
{
	setName(m_query, name);
}
//=============================================================================
void Query::setName(const GLuint& name, const std::string& label)
{
	m_name = label;
	glObjectLabel(m_identifier, name, label.size(), label.c_str());
}
//=============================================================================