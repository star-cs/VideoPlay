
// VideoClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "VideoClient.h"
#include "VideoClientDlg.h"
#include "afxdialogex.h"
#include "VideoClientController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <CTool.h>


// CVideoClientDlg 对话框



CVideoClientDlg::CVideoClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIDEOCLIENT_DIALOG, pParent)
{
	m_BtnPlayStatus = false;
	m_media_length = 0.0f;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PLAY, m_BtnPlay);
	DDX_Control(pDX, IDC_BTN_STOP, m_BtnStop);
	DDX_Control(pDX, IDC_STATIC_MEDIA_POS, m_MediaPos);
	DDX_Control(pDX, IDC_STATIC_VOLUME, m_MediaVolume);
	DDX_Control(pDX, IDC_EDIT_MEDIA_URL, m_MediaUrl);
	DDX_Control(pDX, IDC_SLIDER_POS, m_SliderMediaPos);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_SliderVolume);
	DDX_Control(pDX, IDC_EDIT_MEDIA, m_veido);
}

BEGIN_MESSAGE_MAP(CVideoClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PLAY, &CVideoClientDlg::OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_STOP, &CVideoClientDlg::OnBnClickedBtnStop)
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_VOLUME, &CVideoClientDlg::OnNMCustomdrawSliderVolume)
END_MESSAGE_MAP()


// CVideoClientDlg 消息处理程序

BOOL CVideoClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	
	SetTimer(0, 500, NULL);

	m_SliderMediaPos.SetRange(0, 1);

	m_SliderVolume.SetRange(0, 100);
	m_SliderVolume.SetPos(50);
	m_SliderVolume.SetTicFreq(10);

	SetDlgItemText(IDC_STATIC_VOLUME, _T("50%"));
	SetDlgItemText(IDC_STATIC_MEDIA_POS, _T("--:--:-- / --:--:--"));

	m_controller->SetWnd(m_veido.GetSafeHwnd());

	m_MediaUrl.SetWindowText(_T("rtsp://127.0.0.1:554"));
	//m_MediaUrl.SetWindowText(_T("file:///E:\\Cpp_Projects\\VideoPlay\\VideoPlay\\test.mp4"));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVideoClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVideoClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVideoClientDlg::OnBnClickedBtnPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_BtnPlayStatus == false) {

		CString url;
		m_MediaUrl.GetWindowText(url);
		// TODO:判断当前是否为恢复
		m_controller->SetMedia(CTool::Unicode2Utf8((LPCTSTR)url));

		m_BtnPlay.SetWindowTextW(_T("暂停"));
		m_BtnPlayStatus = true;
		m_controller->VideoCtrl(MVLC_PLAY);
	}
	else {
		m_BtnPlay.SetWindowTextW(_T("播放"));
		m_BtnPlayStatus = false;
		m_controller->VideoCtrl(MVLC_PAUSE);
	}
}


void CVideoClientDlg::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_BtnPlay.SetWindowTextW(_T("播放"));
	m_BtnPlayStatus = false;
	m_controller->VideoCtrl(MVLC_STOP);
}


void CVideoClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 0) {
		float pos = m_controller->VideoCtrl(MVLC_GET_POSITION);	

		if (pos != -1.0f) {

			if (m_SliderMediaPos.GetRangeMax() <= 1) {
				m_media_length = m_controller->VideoCtrl(MVLC_GET_LENGTH);
				m_SliderMediaPos.SetRange(0, m_media_length);
				m_SliderMediaPos.SetTicFreq(20);
			}

			CString strPos;
			strPos.Format(_T("%f / %f"), m_media_length * pos, m_media_length);
			SetDlgItemText(IDC_STATIC_MEDIA_POS, strPos);	
			m_SliderMediaPos.SetPos(int(m_media_length * pos));
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CVideoClientDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 音量
	//TRACE("pos:%d\r\n", nPos);
	if (nSBCode == 5) {
		CString cstr;
		cstr.Format(_T("音量：%d%%"), 100 - nPos);
		SetDlgItemText(IDC_STATIC_VOLUME, cstr);
		m_controller->SetVolume(100 - nPos);
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CVideoClientDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//TRACE("pos:%d\r\n", nPos);
	// 时间
	if (nSBCode == 5) {
		CString cstr;
		cstr.Format(_T("%d / %f"), nPos, m_media_length);
		SetDlgItemText(IDC_STATIC_MEDIA_POS, cstr);
		m_controller->SetPosition((float)nPos / m_media_length);
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}



void CVideoClientDlg::OnNMCustomdrawSliderVolume(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	*pResult = 0;
}
