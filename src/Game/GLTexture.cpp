#include "stdafx.h"
#include "GLScene.h"

bool GL_LoadTextureKTX(GLuint uiTexture, const char* p_cTextureFile)
{
	// Load texture data
	ktxTexture* kTexture;
	KTX_error_code ktxerror = ktxTexture_CreateFromNamedFile(p_cTextureFile,
		KTX_TEXTURE_CREATE_NO_FLAGS,
		&kTexture);
	GLenum GLTarget, GLError;
	ktxerror = ktxTexture_GLUpload(kTexture, &uiTexture, &GLTarget, &GLError);

	// Generate mipmaps
	if (kTexture->numLevels == 1)
		glGenerateMipmap(GL_TEXTURE_2D);

	ktxTexture_Destroy(kTexture);

	// Initialise the texture filtering values
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 4.0f);

	return true;
}