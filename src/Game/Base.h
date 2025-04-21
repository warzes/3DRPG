#pragma once

//=============================================================================
#pragma region [ Base Macros ]

#pragma endregion
//=============================================================================
#pragma region [ Base Func ]

#pragma endregion
//=============================================================================
#pragma region [ Base Template ]

using byte = uint8_t;

class Noncopyable
{
public:
	Noncopyable() = default;
	Noncopyable(const Noncopyable&) = delete;
	Noncopyable& operator=(const Noncopyable&) = delete;
};

#pragma endregion
//=============================================================================