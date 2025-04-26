#pragma once

class Fence final
{
public:
	~Fence();

	void Insert();
	void Wait();

private:
	GLsync m_fence{ nullptr };
};