
// MyDockDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyDock.h"
#include "MyDockDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
// CMyDockDlg 对话框




CMyDockDlg::CMyDockDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyDockDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyDockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyDockDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_WM_NCMOUSEMOVE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMyDockDlg 消息处理程序

BOOL CMyDockDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_screenX = GetSystemMetrics( SM_CXSCREEN );
	m_screenY = GetSystemMetrics( SM_CYSCREEN );
	m_dwLastActiveTime = GetTickCount();
	m_enHidePosi = NO;
	SetTimer( 100, 100, NULL );

	m_bnApp[0].SubclassDlgItem( IDC_BN_APP01, this );
	//m_bnApp[1] = this->GetDlgItem( IDC_BN_APP02 );
	//m_bnApp[2] = this->GetDlgItem( IDC_BN_APP03 );
	//m_bnApp[3] = this->GetDlgItem( IDC_BN_APP04 );
	//m_bnApp[4] = this->GetDlgItem( IDC_BN_APP05 );
	//m_bnApp[5] = this->GetDlgItem( IDC_BN_APP06 );
	//m_bnApp[6] = this->GetDlgItem( IDC_BN_APP07 );
	//m_bnApp[7] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[8] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[9] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[10] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[11] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[12] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[13] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[14] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[15] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[16] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[17] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[18] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[19] = this->GetDlgItem( IDC_BN_APP01 );
	//m_bnApp[20] = this->GetDlgItem( IDC_BN_APP01 );

	LoadSettingFile();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CMyDockDlg::LoadSettingFile(){
	CString strSettingFile;
	CString strKey;
	CString strSection = "Applications";

	GetModuleFileName( NULL, strSettingFile.GetBuffer(MAX_PATH) , MAX_PATH );
	strSettingFile.ReleaseBuffer();
	strSettingFile = strSettingFile.Left( strSettingFile.ReverseFind('\\') ) + "\\setting.ini";
	TRACE( "%s", strSettingFile );
	
	m_vstrApp.resize( MAX_APP_NUM );
	
	for ( int i = 0; i < MAX_APP_NUM; i++ ){
		strKey.Format( "App%02d", i + 1 );
		GetPrivateProfileString( strSection, strKey, "", m_vstrApp.at(i).GetBuffer(MAX_PATH), MAX_PATH, strSettingFile );
		m_vstrApp.at(i).ReleaseBuffer();
	}

	m_bnApp[0].SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME));
	//ShellExecute( NULL, _T("open") , m_vstrApp.at(0), NULL, NULL, SW_SHOWNORMAL );
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyDockDlg::OnPaint()
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
HCURSOR CMyDockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyDockDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_dwLastActiveTime = GetTickCount();

	CDialogEx::OnMouseMove(nFlags, point);
}


void CMyDockDlg::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_dwLastActiveTime = GetTickCount();
	CDialogEx::OnNcMouseMove(nHitTest, point);
}

BOOL CMyDockDlg::IsMouseInWindow( void ){
	CRect rect;
	GetWindowRect(&rect);
	CPoint point;
	GetCursorPos(&point);
	return rect.PtInRect(point);
}

void CMyDockDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch ( nIDEvent ){
	case 100:
		if ( GetTickCount() - m_dwLastActiveTime < 300 ){
			DockedShow();
		} else {
			DockedHidden();
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

#define SEQ_NUM		20
#define EVERY_TIME	( 60 / SEQ_NUM )

void CMyDockDlg::DockedShow( void ){
	
	if( m_enHidePosi != NO && IsMouseInWindow() ){//已隐藏，显示旧值
		//恢复样式
		ModifyStyle( NULL, WS_SYSMENU | WS_THICKFRAME );
		//还原大小
		int seq = 0;
		switch( m_enHidePosi ){
		case TOP:
			while ( ++seq <= SEQ_NUM ){
				this->SetWindowPos( NULL, m_rect.left, m_rect.top, m_rect.right - m_rect.left, ( m_rect.bottom - m_rect.top ) * seq / SEQ_NUM, SWP_NOCOPYBITS );
			    Sleep( EVERY_TIME );
			}
			break;
		case LEFT:
			while ( ++seq <= SEQ_NUM ){
				this->SetWindowPos( NULL, m_rect.left, m_rect.top, ( m_rect.right - m_rect.left ) * seq / SEQ_NUM, m_rect.bottom - m_rect.top, SWP_NOCOPYBITS );
				Sleep( EVERY_TIME );		
			}
			break;
		case RIGHT:
			while ( ++seq <= SEQ_NUM ){
				this->SetWindowPos( NULL, m_rect.left + ( m_rect.right - m_rect.left ) * ( SEQ_NUM - seq ) / SEQ_NUM, m_rect.top, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, SWP_NOCOPYBITS );
				Sleep( EVERY_TIME );
			}
			break;
		}
		m_enHidePosi = NO;
	}
}

void CMyDockDlg::DockedHidden( void ){
	CRect rect;
	GetWindowRect( &rect );
	
	if( m_enHidePosi ==NO && !IsMouseInWindow() ){
		m_rect = rect;
		if ( m_rect.top <=0 ){//靠顶
			m_enHidePosi = TOP;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, m_rect.left, 0, m_rect.right - m_rect.left, 2, SWP_NOCOPYBITS );
			//下移
			//右移动
			m_rect.bottom -= m_rect.top;
			m_rect.top    =  0;
			
		} else if ( m_rect.left <=0 ){ //靠左
			m_enHidePosi = LEFT;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, 0, m_rect.top, 2, m_rect.bottom - m_rect.top, SWP_NOCOPYBITS );
			//右移动
			m_rect.right -= m_rect.left;
			m_rect.left  = 0;
			
		} else if ( m_rect.right >= m_screenX ){ //靠右
			m_enHidePosi = RIGHT;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, m_screenX-2, m_rect.top, 2, m_rect.bottom-m_rect.top, SWP_NOCOPYBITS );
			//左移动
			m_rect.left  = m_screenX - ( m_rect.right - m_rect.left );
			m_rect.right = m_screenX;//m_rect.left; //偏移量   m_rect.right - m_screenX
			
		} else {
			m_enHidePosi = NO;
		}	
	}
}



void CMyDockDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


void CMyDockDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnCancel();
}

BOOL CMyDockDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE ){
		//m_obj.SetFocus();
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
