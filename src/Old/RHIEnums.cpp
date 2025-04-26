#include "stdafx.h"
#include "RHIEnums.h"
//=============================================================================
unsigned int ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:  return sizeof(float);
	case ShaderDataType::Float2: return sizeof(float) * 2;
	case ShaderDataType::Float3: return sizeof(float) * 3;
	case ShaderDataType::Float4: return sizeof(float) * 4;
	case ShaderDataType::Mat3:   return sizeof(float) * 3 * 3;
	case ShaderDataType::Mat4:   return sizeof(float) * 4 * 4;
	case ShaderDataType::Int:    return sizeof(int);
	case ShaderDataType::Int2:   return sizeof(int) * 2;
	case ShaderDataType::Int3:   return sizeof(int) * 3;
	case ShaderDataType::Int4:   return sizeof(int) * 4;
	case ShaderDataType::Bool:   return 1;
	}
	return 0;
}
//=============================================================================
unsigned int GetComponentCount(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:  return 1;
	case ShaderDataType::Float2: return 2;
	case ShaderDataType::Float3: return 3;
	case ShaderDataType::Float4: return 4;
	case ShaderDataType::Mat3:   return 3 * 3;
	case ShaderDataType::Mat4:   return 4 * 4;
	case ShaderDataType::Int:    return 1;
	case ShaderDataType::Int2:   return 2;
	case ShaderDataType::Int3:   return 3;
	case ShaderDataType::Int4:   return 4;
	case ShaderDataType::Bool:   return 1;
	}
	return 0;
}
//=============================================================================
GLenum GetShaderDataType(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float:
	case ShaderDataType::Float2:
	case ShaderDataType::Float3:
	case ShaderDataType::Float4:
	case ShaderDataType::Mat3:
	case ShaderDataType::Mat4:
		return GL_FLOAT;
	case ShaderDataType::Int:
	case ShaderDataType::Int2:
	case ShaderDataType::Int3:
	case ShaderDataType::Int4:
		return GL_INT;
	case ShaderDataType::Bool:
		return GL_BOOL;
	case ShaderDataType::None:
	default:
		return 0;
	}
}
//=============================================================================