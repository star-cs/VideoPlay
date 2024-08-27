#include "pch.h"
#include "MVlc.h"

MVlc::MVlc()
{
	m_instance = libvlc_new(0, NULL);
	m_media = NULL;
	m_player = NULL;
	m_hWnd = NULL;
}

MVlc::~MVlc()
{
	if (m_player != NULL) {
		libvlc_media_player_t* temp = m_player;
		m_player = NULL;
		libvlc_media_player_release(temp);
	}

	if (m_media != NULL) {
		libvlc_media_t* temp = m_media;
		m_media = NULL;
		libvlc_media_release(temp);
	}

	if (m_instance != NULL) {
		libvlc_instance_t* temp = m_instance;
		m_instance = NULL;
		libvlc_release(temp);
	}

}

// 返回1，新旧url相同，返回0，新旧url不同。
int MVlc::SetMedia(std::string strUrl)
{
	if (m_instance == NULL || (m_hWnd == NULL)) return -1;
	if (m_url == strUrl) return 1;	// 通过比较文件url，确认当前是否需要重新加载media，player。第一次SetMedia，m_url为空
	m_url = strUrl;

	if (m_media != NULL) {
		libvlc_media_t* temp = m_media;
		m_media = NULL;
		libvlc_media_release(temp);
	}
	m_media = libvlc_media_new_location(m_instance, strUrl.c_str());
	if (m_media == NULL) return -2;

	if (m_player != NULL) {
		libvlc_media_player_t* temp = m_player;
		m_player = NULL;
		libvlc_media_player_release(temp);
	}
	m_player = libvlc_media_player_new_from_media(m_media);
	if (m_player == NULL) return -3;

	// player media instance 都创建成功
	libvlc_media_player_set_hwnd(m_player, m_hWnd);
	return 0;
}

// 先调用 SetMedia再设置
int MVlc::SetHWnd(HWND hWnd)
{
	m_hWnd = hWnd;
	return 0;
}

int MVlc::Play()
{
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return -1;
	return libvlc_media_player_play(m_player);
}

int MVlc::Pause()
{
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return -1;
	libvlc_media_player_pause(m_player);
	return 0;
}

int MVlc::Stop()
{
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return -1;
	libvlc_media_player_stop(m_player);
	return 0;
}

float MVlc::GetPostion()
{
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return -1.0f;
	return libvlc_media_player_get_position(m_player);
}

int MVlc::SetPostion(float pos)
{
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return -1;
	libvlc_media_player_set_position(m_player, pos);
	return 0;
}

int MVlc::GetVolume()
{
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return -1;
	return libvlc_audio_get_volume(m_player);
}

int MVlc::SetVolume(int volume)
{
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return -1;
	return libvlc_audio_set_volume(m_player, volume);
}

MVlcSize MVlc::GetMediaInfo()
{
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return  MVlcSize(-1, -1);

	return MVlcSize(libvlc_video_get_height(m_player), libvlc_video_get_width(m_player));
}

float MVlc::GetMediaLength()
{
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return  -1;

	libvlc_time_t tm = libvlc_media_player_get_length(m_player);
	float ret = tm / 1000.0f;
	return ret;
}

