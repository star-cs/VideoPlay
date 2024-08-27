#include "pch.h"
#include "MVlc.h"

MVlc::MVlc()
{
	m_instance = libvlc_new(0, NULL);
	m_media = NULL;
	m_player = NULL;
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
		libvlc_release(m_instance);
	}

}

int MVlc::SetMedia(std::string strUrl)
{
	if (m_instance == NULL) return -1;

	m_media = libvlc_media_new_location(m_instance, strUrl.c_str());
	if (m_media == NULL) return -2;
	
	m_player = libvlc_media_player_new_from_media(m_media);
	if (m_player == NULL) return -3;


	return 0;
}


int MVlc::SetHWnd(HWND hWnd)
{
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return -1;
	libvlc_media_player_set_hwnd(m_player, hWnd);
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
	if (m_player == NULL || m_instance == NULL || m_media == NULL) return -1.0;
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

