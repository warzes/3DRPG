#pragma once

/*
TODO:
	SetYCoCg/GetYCoCg
	SetHSL/GetHSL
	SetCMYK/GetCMYK
	Color3 ToSRGB()/ Color3 ToSRGB
*/

class Color3 final
{
public:
	Color3() = default;
	constexpr explicit Color3(float value) : r(value), g(value), b(value) {};
	constexpr Color3(float R, float G, float B) : r(R), g(G), b(B) {};
	explicit Color3(uint8_t R, uint8_t G, uint8_t B);
	explicit Color3(uint32_t dword);

	float* Ptr() { return &r; }
	const float* Ptr() const { return &r; }
	operator float* () { return &r; }

	constexpr Color3 operator*(const Color3& rhs) const { return { r * rhs.r, g * rhs.g, b * rhs.b }; }
	constexpr Color3 operator/(const Color3& rhs) const { return { r / rhs.r, g / rhs.g, b / rhs.b }; }
	constexpr Color3 operator*(float rhs) const { return { r * rhs, g * rhs, b * rhs }; }
	constexpr Color3 operator/(float rhs) const { return (*this) * (1.0f / rhs); }

	Color3& operator*=(const Color3& rhs)
	{
		r *= rhs.r;
		g *= rhs.g;
		b *= rhs.b;
		return *this;
	}
	Color3& operator/=(const Color3& rhs)
	{
		r /= rhs.r;
		g /= rhs.g;
		b /= rhs.b;
		return *this;
	}
	Color3& operator*=(float rhs)
	{
		r *= rhs;
		g *= rhs;
		b *= rhs;
		return *this;
	}
	Color3& operator/=(float rhs)
	{
		float invRhs = 1.0f / rhs;
		r *= invRhs;
		g *= invRhs;
		b *= invRhs;
		return *this;
	}

	void     SetByte(uint8_t r, uint8_t g, uint8_t b);
	void     GetByte(uint8_t& r, uint8_t& g, uint8_t& b) const;
	void     SetDWord(uint32_t color);
	uint32_t GetDWord() const;
	
	float r{ 1.0f };
	float g{ 1.0f };
	float b{ 1.0f };
};

class Color4 final
{
public:
	Color4() = default;
	constexpr explicit Color4(float value) : r(value), g(value), b(value), a(value) {};
	constexpr Color4(float R, float G, float B, float A = 1.0f) : r(R), g(G), b(B), a(A) {};
	explicit Color4(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255);
	explicit Color4(uint32_t dword);

	float* Ptr() { return &r; }
	const float* Ptr() const { return &r; }
	operator float* () { return &r; }

	constexpr Color4 operator*(const Color4& rhs) const { return { r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a }; }
	constexpr Color4 operator/(const Color4& rhs) const { return { r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a }; }
	constexpr Color4 operator*(float rhs) const { return { r * rhs, g * rhs, b * rhs, a * rhs }; }
	constexpr Color4 operator/(float rhs) const { return (*this) * (1.0f / rhs, a / rhs); }

	Color4& operator*=(const Color4& rhs)
	{
		r *= rhs.r;
		g *= rhs.g;
		b *= rhs.b;
		a *= rhs.a;
		return *this;
	}
	Color4& operator/=(const Color4& rhs)
	{
		r /= rhs.r;
		g /= rhs.g;
		b /= rhs.b;
		a /= rhs.a;
		return *this;
	}
	Color4& operator*=(float rhs)
	{
		r *= rhs;
		g *= rhs;
		b *= rhs;
		a *= rhs;
		return *this;
	}
	Color4& operator/=(float rhs)
	{
		float invRhs = 1.0f / rhs;
		r *= invRhs;
		g *= invRhs;
		b *= invRhs;
		a *= invRhs;
		return *this;
	}

	void     SetByte(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
	void     GetByte(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const;
	void     SetDWord(uint32_t color);
	uint32_t GetDWord() const;

	float r{ 1.0f };
	float g{ 1.0f };
	float b{ 1.0f };
	float a{ 1.0f };
};