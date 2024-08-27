#include "pch.h"
#include "VideoClientController.h"

VideoClientController::VideoClientController()
{
	// ����һ��ʵ�ַ�ʽ��dlg�д���һ��controllerָ��                                                        
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
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
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
