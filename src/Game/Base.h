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
#pragma region [ Color ]

class Color3 final
{
public:
	Color3() = default;
	constexpr explicit Color3(float value) : r(value), g(value), b(value) {};
	constexpr Color3(float R, float G, float B) : r(R), g(G), b(B) {};

	float* Ptr() { return &r; }
	const float* Ptr() const { return &r; }
	operator float*() { return &r; }

	horc


	float r{ 1.0f };
	float g{ 1.0f };
	float b{ 1.0f };
};

#pragma endregion
