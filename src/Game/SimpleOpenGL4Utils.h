#pragma once

namespace sample
{

	inline std::string ShaderTypeToString(GLenum shaderType)
	{
		switch (shaderType)
		{
		case GL_VERTEX_SHADER:          return "GL_VERTEX_SHADER";
		case GL_FRAGMENT_SHADER:        return "GL_FRAGMENT_SHADER";
		case GL_GEOMETRY_SHADER:        return "GL_GEOMETRY_SHADER";
		case GL_TESS_CONTROL_SHADER:    return "GL_TESS_CONTROL_SHADER";
		case GL_TESS_EVALUATION_SHADER: return "GL_TESS_EVALUATION_SHADER";
		case GL_COMPUTE_SHADER:         return "GL_COMPUTE_SHADER";
		default:                        return "UNKNOWN_SHADER_TYPE";
		}
	}

	inline GLenum GetBaseFormat(GLenum internalFormat)
	{
		switch (internalFormat)
		{
		case GL_R8:
		case GL_R8_SNORM:
		case GL_R16:
		case GL_R16_SNORM:
		case GL_R16F:
		case GL_R32F:
		case GL_R8I:
		case GL_R8UI:
		case GL_R16I:
		case GL_R16UI:
		case GL_R32I:
		case GL_R32UI:
			return GL_RED;

		case GL_RG8:
		case GL_RG8_SNORM:
		case GL_RG16:
		case GL_RG16_SNORM:
		case GL_RG16F:
		case GL_RG32F:
		case GL_RG8I:
		case GL_RG8UI:
		case GL_RG16I:
		case GL_RG16UI:
		case GL_RG32I:
		case GL_RG32UI:
			return GL_RG;

		case GL_R3_G3_B2:
		case GL_RGB4:
		case GL_RGB5:
		case GL_RGB8:
		case GL_RGB8_SNORM:
		case GL_RGB10:
		case GL_RGB12:
		case GL_RGB16_SNORM:
		case GL_RGBA2:
		case GL_RGBA4:
		case GL_SRGB8:
		case GL_RGB16F:
		case GL_RGB32F:
		case GL_R11F_G11F_B10F:
		case GL_RGB9_E5:
		case GL_RGB8I:
		case GL_RGB8UI:
		case GL_RGB16I:
		case GL_RGB16UI:
		case GL_RGB32I:
		case GL_RGB32UI:
			return GL_RGB;

		case GL_RGB5_A1:
		case GL_RGBA8:
		case GL_RGBA8_SNORM:
		case GL_RGB10_A2:
		case GL_RGB10_A2UI:
		case GL_RGBA12:
		case GL_RGBA16:
		case GL_SRGB8_ALPHA8:
		case GL_RGBA16F:
		case GL_RGBA32F:
		case GL_RGBA8I:
		case GL_RGBA8UI:
		case GL_RGBA16I:
		case GL_RGBA16UI:
		case GL_RGBA32I:
		case GL_RGBA32UI:
			return GL_RGBA;

		case GL_DEPTH_COMPONENT16:
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32F:
			return GL_DEPTH_COMPONENT;

		case GL_DEPTH24_STENCIL8:
		case GL_DEPTH32F_STENCIL8:
			return GL_DEPTH_STENCIL;

		case GL_STENCIL_INDEX8:
			return GL_STENCIL_INDEX;
		}

		return 0;
	}

	struct TextureParameter final
	{
		// TODO:
	};
}