#pragma once

#include "Base.h"

enum MediaType
{
	MEDIA_TYPE_H264 = 96,
	MEDIA_TYPE_AAC = 97,
	MEDIA_TYPE_MP3 = 14,
};

class MediaFile
{
public:
	MediaFile();
	~MediaFile();

	int Open(const MBuffer& filePath, int nType = MEDIA_TYPE_H264);

	// ���buffer��sizeΪ0�����ʾû��֡�ˡ�
	MBuffer ReadOneFrame();

	void Close();

	// ���ú�ReadOneFrame()�����´��ļ�ͷ��ʼ��ȡ��
	void Reset();

private:
	MBuffer ReadH264Frame();

	// ����-1����ʧ�ܡ�
	int FindH264Header();

private:
	long m_size;
	FILE* m_file;
	MBuffer m_filePath;
	int m_type;
};

