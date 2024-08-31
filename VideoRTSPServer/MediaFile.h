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

	// 如果buffer的size为0，则表示没有帧了。
	MBuffer ReadOneFrame();

	void Close();

	// 重置后，ReadOneFrame()会重新从文件头开始读取。
	void Reset();

private:
	MBuffer ReadH264Frame();

	// 返回-1查找失败。
	int FindH264Header();

private:
	long m_size;
	FILE* m_file;
	MBuffer m_filePath;
	int m_type;
};

