#pragma once

//=============================================================================
// enums
//=============================================================================

enum class ShaderDataType : uint8_t
{
	None = 0,
	Float,
	Float2,
	Float3,
	Float4,
	Mat3,
	Mat4,
	Int,
	Int2,
	Int3,
	Int4,
	Bool
};

unsigned int ShaderDataTypeSize(ShaderDataType type);
unsigned int GetComponentCount(ShaderDataType type);
GLenum GetShaderDataType(ShaderDataType type);

//=============================================================================
// structs
//=============================================================================

struct VertexBufferElement final
{
	VertexBufferElement() = default;
	VertexBufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
		: type(type)
		, name(name)
		, size(ShaderDataTypeSize(type))
		, offset(0)
		, normalized(normalized)
	{
	}

	ShaderDataType type;
	std::string    name;
	unsigned int   size;
	unsigned int   offset;
	bool           normalized;
};