#pragma once

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

enum class ImageFormat : uint8_t
{
	Grey,
	GreyAlpha,
	RGB,
	RGBA
};