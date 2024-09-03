#pragma once
#include <Windows.h>

class CTool
{
public:
	static void MTrace(const char* format, ...);


	static void Dump(BYTE* pData, size_t nSize);
};

