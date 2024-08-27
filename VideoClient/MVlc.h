#pragma once
#include <string>
#include "vlc.h"

class MVlcSize 
{
public:
	int m_height;
	int m_width;

	MVlcSize(int h, int w) {
		m_height = h;
		m_width = w;
	}

	MVlcSize& operator=(MVlcSize vlc) {
		if (this != &vlc) {
			m_height = vlc.m_height;
			m_width = vlc.m_width;
		}
		return *this;

	}

	MVlcSize(const MVlcSize& vlc) {
		m_height = vlc.m_height;
		m_width = vlc.m_width;
	}
};

class MVlc
{
public:
	MVlc();

	~MVlc();

	// strUrl如果包含中文，请传入utf-8的编码格式的字符串
	int SetMedia(std::string strUrl);
	int SetHWnd(HWND hWnd);

	int Play();
	int Pause();
	int Stop();

	float GetPostion();
	int SetPostion(float pos);

	int GetVolume();
	int SetVolume(int volume);

	MVlcSize GetMediaInfo();

	float GetMediaLength();
private:
	libvlc_instance_t* m_instance;
	libvlc_media_t* m_media;
	libvlc_media_player_t* m_player; 

	std::string m_url;

#ifdef WIN32
	HWND m_hWnd;
#endif
};

