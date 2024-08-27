#include "pch.h"
#include "VideoClientController.h"

VideoClientController::VideoClientController()
{
	// 单例一个实现方式，dlg中存在一个controller指针                                                        
	m_dlg.m_controller = this;
}

VideoClientController::~VideoClientController()
{
	
}

int VideoClientController::Init(CWnd* pWnd)
{
	pWnd = &m_dlg;
	return 0;
}

int VideoClientController::Invoke()
{
	INT_PTR nResponse = m_dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}
	return nResponse;
}

int VideoClientController::SetMedia(const std::string& strUrl)
{ 
	return m_vlc.SetMedia(strUrl);
}

float VideoClientController::ViedoCtrl(MVlcCommand cmd)
{
	/*
	MVLC_PLAY,
	MVLC_PAUSE,
	MVLC_STOP,
	MVLC_GET_VOLUME,
	MVLC_GET_POSITION,
	MVLC_GET_LENGTH,
	*/
	switch (cmd)
	{
	case MVLC_PLAY:
		return m_vlc.Play();

	case MVLC_PAUSE:
		return m_vlc.Pause();

	case MVLC_STOP:
		return m_vlc.Stop();
		
	case MVLC_GET_VOLUME:
		return m_vlc.GetVolume();
	
	case MVLC_GET_POSITION:
		return m_vlc.GetPostion();

	case MVLC_GET_LENGTH:
		return m_vlc.GetMediaLength();

	default:
		break;
	}
	return -1.0f;
}

void VideoClientController::SetPosition(float pos)
{
	m_vlc.SetPostion(pos);
}

int VideoClientController::SetWnd(HWND hWnd)
{
	return m_vlc.SetHWnd(hWnd);
}

int VideoClientController::SetVolume(int volume)
{
	return m_vlc.SetVolume(volume);

}

MVlcSize VideoClientController::GetMediaInfo()
{
	return m_vlc.GetMediaInfo();
}
