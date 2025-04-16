#include "stdafx.h"
#include "RHIFence.h"
//=============================================================================
//=============================================================================
Fence::~Fence()
{
	if (m_fence)
		glDeleteSync(m_fence);
}
//=============================================================================
void Fence::Insert()
{
	if (m_fence)
		Wait();

	m_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}
//=============================================================================
void Fence::Wait()
{
	if (m_fence)
	{
		glClientWaitSync(m_fence, 0, 10000000);
		m_fence = nullptr;
		glDeleteSync(m_fence);
	}
}
//=============================================================================