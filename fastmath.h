
#pragma once

#include "attributes.h"

// http://www.research.scea.com/gdc2003/fast-math-functions.html

constexpr float PI = 3.1415926535897932384626433832795f;

attr_pure
constexpr float sinhelper(float x)
{
	// x in [0, pi/2]
	// x + A * x^3 + B * x^5 + C * x^7

	// taylor
#if 0
	const float A = -0.16667f;
	const float B = 0.0083333f;
	const float C = -0.00019841f;
#else
	// minimax coeffs
	const float A = -0.16666f;
	const float B = 0.0083143f;
	const float C = -0.00018542f;
#endif

	float z = x*x;
	// Horner:
	// (((C * z + B) * z + A) * z + 1)*x
	// only 3 registers

	// Estrin's method
	// f(x) = D*x^3 + C*x^2 +  B*x + A
	//      = (D*x + C)*x^2 + (B*x + A)
	//      = P*x^2 + Q

	return (C*x*z + B*x)*(z*z)  + (A*x*z + x);
}

attr_pure
float fastsinf(float x)
{
	// working range [0, pi/2]
	const float C =  PI / 2;
	const float invC = 1 / C;

	// truncate
	int k = (int)(x * invC);
	// additive range reduction
	float y = x - k * C;

	// lower bits of k hold quadrant
	switch (k & 3)
	{
	case 0:
		return sinhelper(y);
	case 1:
		return sinhelper(C - y);
	case 2:
		return -sinhelper(C - y);
	default:
		return -sinhelper(y);
	}
}

struct SinCos
{
	float s = 0;
	float c = 0;
};

attr_pure
constexpr SinCos fastsincosf(float x)
{
	// working range [0, pi/2]
	const float C =  PI / 2;
	const float invC = 1 / C;

	// truncate
	int k = (int)(x * invC);
	// additive range reduction
	float y = x - k * C;

	float s = sinhelper(y);
	float c = sinhelper(C - y);

	// lower bits of k hold quadrant
	switch (k & 3)
	{
	case 0:
		return{s, c};
	case 1:
		return{c, -s};
	case 2:
		return{-s, -c};
	default:
		return{-c, s};
	}
}
