#pragma once
#include <string>
#include "Windows.h"
class CTool
{
public:
	static std::string Unicode2Utf8(const std::wstring& strIn)
	{
		std::string str;
		int length = ::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), NULL, 0, NULL, NULL);
		str.resize(length + 1);
		::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), (LPSTR)str.c_str(), length, NULL, NULL);
		return str;
	}

};

