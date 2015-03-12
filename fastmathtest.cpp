
// clang++ -std=c++14 -march=corei7 -O3 -fslp-vectorize-aggressive -Rpass=".*" -Rpass-analysis=".*" -Rpass-missed=".*" -masm=intel fastmathtest.cpp -o test.exe

#include "fastmath.h"

extern "C" int printf(const char* fmt, ...);
int main()
{
	#pragma clang loop vectorize(enable)
	for (int i = 0; i <= 256; ++i)
	{
		float input = i * (2*PI) / 256;
		auto sc = fastsincosf(input);
		printf("%.9g %.9g %.9g\n", input, sc.s, sc.c);
	}

	return 0;
}