#pragma once

class Shader final
{
	friend class Program;

public:
	~Shader();

	using Ptr = std::shared_ptr<Shader>;

	static Shader::Ptr Create(GLenum type, const std::string& source);
	static Shader::Ptr CreateFromFile(GLenum type, const std::string& path, const std::vector<std::string>& defines = {});

	GLenum Type() const;
	bool   Compiled() const;
	GLuint Id() const;

	void SetName(const std::string& name);

private:
	Shader(GLenum type, const std::string& source);

	void setName(const GLuint& name, const std::string& label);

	bool         m_compiled;
	GLuint       m_glShader;
	GLenum       m_type;
	const GLenum m_identifier{ GL_SHADER };
	std::string  m_name;
};

class Program final
{
public:
	struct UniformReflection final
	{
		uint32_t    location;
		std::string name;
		GLenum      type;
	};

	struct SamplerReflection final
	{
		uint32_t    location;
		std::string name;
		GLenum      type;
	};

	struct ImageReflection final
	{
		uint32_t    location;
		std::string name;
		GLenum      type;
	};

	struct UBOReflection final
	{
		uint32_t    binding;
		std::string name;
	};

	struct SSBOReflection final
	{
		uint32_t    binding;
		std::string name;
	};

	struct ReflectionData final
	{
		std::unordered_map<uint32_t, UniformReflection> uniforms;
		std::unordered_map<uint32_t, SamplerReflection> samplers;
		std::unordered_map<uint32_t, ImageReflection>   images;
		std::unordered_map<uint32_t, UBOReflection>     ubos;
		std::unordered_map<uint32_t, SSBOReflection>    ssbos;
	};

	~Program();

	using Ptr = std::shared_ptr<Program>;

	static Program::Ptr Create(std::vector<Shader::Ptr> shaders);

	void    Use();
	int32_t NumActiveUniformBlocks() const;
	void    UniformBlockBinding(std::string name, int binding);
	bool    SetUniform(std::string name, int32_t value);
	bool    SetUniform(std::string name, uint32_t value);
	bool    SetUniform(std::string name, float value);
	bool    SetUniform(std::string name, const glm::vec2& value);
	bool    SetUniform(std::string name, const glm::vec3& value);
	bool    SetUniform(std::string name, const glm::vec4& value);
	bool    SetUniform(std::string name, const glm::mat2& value);
	bool    SetUniform(std::string name, const glm::mat3& value);
	bool    SetUniform(std::string name, const glm::mat4& value);
	bool    SetUniform(std::string name, int count, int* value);
	bool    SetUniform(std::string name, int count, float* value);
	bool    SetUniform(std::string name, int count, glm::vec2* value);
	bool    SetUniform(std::string name, int count, glm::vec3* value);
	bool    SetUniform(std::string name, int count, glm::vec4* value);
	bool    SetUniform(std::string name, int count, glm::mat2* value);
	bool    SetUniform(std::string name, int count, glm::mat3* value);
	bool    SetUniform(std::string name, int count, glm::mat4* value);
	void    ExtractReflectionData(ReflectionData& reflectionData);
	
	GLint   Id() const;

	void SetName(const std::string& name);

private:
	Program(std::vector<Shader::Ptr> shaders);

	void setName(const GLuint& name, const std::string& label);

	GLuint                                  m_glProgram;
	int32_t                                 m_numActiveUniformBlocks;
	std::unordered_map<std::string, GLuint> m_locationMap;
	const GLenum                            m_identifier{ GL_PROGRAM };
	std::string                             m_name;
};