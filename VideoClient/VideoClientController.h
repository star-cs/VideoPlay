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
	// 如果strUrl中包含中文符号，则使用utf-8编码
	int SetMedia(const std::string& strUrl);
	// 返回-1.0表示错误。
	float VideoCtrl(MVlcCommand cmd);
	
	void SetPosition(float pos);
	int SetWnd(HWND hWnd);
	int SetVolume(int volume);
	MVlcSize GetMediaInfo();
	 
protected:
	MVlc m_vlc;
	CVideoClientDlg m_dlg;
};

