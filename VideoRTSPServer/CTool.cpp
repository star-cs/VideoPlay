#include "CTool.h"
#include <varargs.h>
#include <Windows.h>
#include <string>

void CTool::MTrace(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);	// 初始化可变参数列表 ap，告诉编译器从 format 之后的位置开始读取可变参数。
	std::string sBuffer;
	sBuffer.resize(10240);
	vsprintf((char*)sBuffer.c_str(), format, ap);	// 使用 vsprintf 函数将格式化的字符串写入 sBuffer。
	// 这里将 sBuffer.c_str() 强制转换为 char* 类型，以便 vsprintf 可以正确地处理它。
	OutputDebugStringA(sBuffer.c_str());
	va_end(ap);
}

void CTool::Dump(BYTE* pData, size_t nSize)
{
    std::string strOut;
    for (size_t i = 0; i < nSize; i++) {
        char buf[8] = "";
        if (i % 16 == 0) strOut += "\n";
        snprintf(buf, sizeof(buf), "%02X ", pData[i] & 0xFF);
        strOut += buf;
    }
    strOut += "\n";
    OutputDebugStringA(strOut.c_str());
}
