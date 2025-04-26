#pragma once

#include "RHIEnums.h"

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