#pragma once
#include "VideoClientDlg.h"
#include "MVlc.h"

enum MVlcCommand {
	MVLC_PLAY,
	MVLC_PAUSE,
	MVLC_STOP,
	MVLC_GET_VOLUME,
	MVLC_GET_POSITION,
	MVLC_GET_LENGTH
};

class VideoClientController
{
public:
	VideoClientController();
	~VideoClientController();

	int Init(CWnd* pWnd);
	int Invoke();
	// ���strUrl�а������ķ��ţ���ʹ��utf-8����
	int SetMedia(const std::string& strUrl);
	// ����-1.0��ʾ����
	float VideoCtrl(MVlcCommand cmd);
	
	void SetPosition(float pos);
	int SetWnd(HWND hWnd);
	int SetVolume(int volume);
	MVlcSize GetMediaInfo();
	 
protected:
	MVlc m_vlc;
	CVideoClientDlg m_dlg;
};

