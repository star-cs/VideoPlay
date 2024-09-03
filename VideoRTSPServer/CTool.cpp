#include "CTool.h"
#include <varargs.h>
#include <Windows.h>
#include <string>

void CTool::MTrace(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);	// ��ʼ���ɱ�����б� ap�����߱������� format ֮���λ�ÿ�ʼ��ȡ�ɱ������
	std::string sBuffer;
	sBuffer.resize(10240);
	vsprintf((char*)sBuffer.c_str(), format, ap);	// ʹ�� vsprintf ��������ʽ�����ַ���д�� sBuffer��
	// ���ｫ sBuffer.c_str() ǿ��ת��Ϊ char* ���ͣ��Ա� vsprintf ������ȷ�ش�������
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
