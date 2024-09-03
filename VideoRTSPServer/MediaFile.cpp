#include "MediaFile.h"

MediaFile::MediaFile(): m_file(nullptr), m_type(-1)
{

}

MediaFile::~MediaFile()
{
	Close();
}

int MediaFile::Open(const MBuffer& filePath, int nType)
{
	
	m_file = fopen(filePath, "rb");
	if (m_file == NULL) {
		printf("open file error!\n");
		return -1;
	}

	m_type = nType;
	m_filePath = filePath;
	fseek(m_file, 0, SEEK_END);
	m_size = ftell(m_file);
	fseek(m_file, 0, SEEK_SET);
	return 0;
}

MBuffer MediaFile::ReadOneFrame()
{
	switch (m_type) {
	case MEDIA_TYPE_H264:
		return ReadH264Frame();
	default:
		break;
	}
	return MBuffer();
}

void MediaFile::Close()
{
	if (m_file) {
		FILE* pFile = m_file;
		m_file = NULL;
		fclose(pFile);
	} 
}

void MediaFile::Reset()
{
	if (m_file) {
		fseek(m_file, 0, SEEK_SET);
	}
}

MBuffer MediaFile::ReadH264Frame()
{
	if (m_file) {
		int headSize = 0;
		long off = FindH264Header(headSize);			
		if(off == -1) return MBuffer();
		fseek(m_file, off + headSize, SEEK_SET);	// 跳过00 00 00 01，接下来找 尾部。
		long tail = FindH264Header(headSize);
		if (tail == -1) {	// 下一个头部没找到，说明是文件尾部。
			tail = m_size;
		}
		long size = tail - 1 - off;
		
		fseek(m_file, off, SEEK_SET);
		MBuffer result(size);
		fread(result, 1, size, m_file);

		return result;
	}
	return MBuffer();
}

// 结束时，m_file指向每一帧的开头 00 00 00 01 或 00 00 01 的第一个00位置。
long MediaFile::FindH264Header(int& headSize)
{
	 while (!feof(m_file)) {
		char buf[4];
		fread(buf, 1, 4, m_file);
		if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x01) {
			headSize = 4;
			fseek(m_file, -3, SEEK_CUR);
			return ftell(m_file);
		}
		else if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x01) {
			headSize = 3;
			fseek(m_file, -3, SEEK_CUR);
			return ftell(m_file);
		}
		fseek(m_file, -3, SEEK_CUR);
	}
	return -1;
}
