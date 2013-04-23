
// MyDockDlg.cpp : 
//

#include "stdafx.h"
#include "MyDock.h"
#include "MyDockDlg.h"
#include "afxdialogex.h"

#include <shlobj.h>
#include <shlguid.h>
#include <shellapi.h>
#include <commctrl.h>
#include <commoncontrols.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;




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
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BN_HEAD, IDC_BN_END, &CMyDockDlg::OnBnClickedBnApp)
END_MESSAGE_MAP()



BOOL CMyDockDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	SetIcon(m_hIcon, TRUE);			// 
	SetIcon(m_hIcon, FALSE);		// 

	// TODO: 
	m_screenX = GetSystemMetrics( SM_CXSCREEN );
	m_screenY = GetSystemMetrics( SM_CYSCREEN );
	m_dwLastActiveTime = GetTickCount();
	m_enHidePosi = NO;
	SetTimer( 100, 100, NULL );

	OleInitialize( NULL );

	Loading();

	return TRUE;  
}

CMyDockDlg::~CMyDockDlg(){
	
	for (std::vector<CButton*>::iterator i = m_vpbnApp.begin(); i != m_vpbnApp.end(); i++ ){
		delete *i;
	}
}

void CMyDockDlg::Loading( void ){
	CString strSettingFile;
	CString strKey;
	CString strSection = "Applications";
	UINT nSettingItemCnt = 0;
	UINT i;

	GetModuleFileName( NULL, strSettingFile.GetBuffer(MAX_PATH) , MAX_PATH );
	strSettingFile.ReleaseBuffer();
	strSettingFile = strSettingFile.Left( strSettingFile.ReverseFind('\\') ) + "\\setting.ini";
	TRACE( "%s\n", strSettingFile );
	
	m_vstrApp.resize( MAX_APP_NUM );
	
	for ( i = 0; i < MAX_APP_NUM; i++ ){
		strKey.Format( "App%d", i + 1 );
		GetPrivateProfileString( strSection, strKey, "", m_vstrApp.at(i).GetBuffer(MAX_PATH), MAX_PATH, strSettingFile );
		m_vstrApp.at(i).ReleaseBuffer();
		
		if ( !m_vstrApp.at(i).IsEmpty() ){
			nSettingItemCnt++;
			TRACE( "%s:%s\n", strKey, m_vstrApp.at(i) );
		}
	}

	m_vpbnApp.resize( nSettingItemCnt );

	// Get the icon index using SHGetFileInfo
	SHFILEINFO sfi = {0};
	SHGetFileInfo(m_vstrApp.at(0), -1, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);

	// Retrieve the system image list.
	// To get the 48x48 icons, use SHIL_EXTRALARGE
	// To get the 256x256 icons (Vista only), use SHIL_JUMBO
	HIMAGELIST* imageList;
	HRESULT hResult = SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&imageList);

	if (hResult == S_OK) {
	  // Get the icon we need from the list. Note that the HIMAGELIST we retrieved
	  // earlier needs to be casted to the IImageList interface before use.
	  HICON hIcon;
	  hResult = ((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);

	  if (hResult == S_OK) {
		// Do something with the icon here.
		// For example, in wxWidgets:
		  i = 0;
		m_vpbnApp.at(i) = new CButton;


		m_vpbnApp.at(i)->Create( "1", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect( 50, 50, 150, 150 ), this, IDC_BN_HEAD + i );
		m_vpbnApp.at(i)->SetIcon( hIcon );
	  }
	}

	SHFILEINFO stSfi = { 0 };

	for ( i = 0; i < nSettingItemCnt; i++ ){
		m_vpbnApp.at(i) = new CButton;

		SHGetFileInfo( m_vstrApp.at(i), NULL, &stSfi, sizeof(SHFILEINFO), SHGFI_ICON );
		m_vpbnApp.at(i)->Create( "1", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect( i*50, i*50, (i+1)*50, (i+1)*50 ), this, IDC_BN_HEAD + i );
		m_vpbnApp.at(i)->SetIcon( stSfi.hIcon );
	}
//	m_bnApp[0].SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME));
	//
}

void CMyDockDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);


		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;


		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMyDockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyDockDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	m_dwLastActiveTime = GetTickCount();

	CDialogEx::OnMouseMove(nFlags, point);
}


void CMyDockDlg::OnNcMouseMove(UINT nHitTest, CPoint point)
{
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
	
	if( m_enHidePosi != NO && IsMouseInWindow() ){

		ModifyStyle( NULL, WS_SYSMENU | WS_THICKFRAME );

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
		if ( m_rect.top <=0 ){
			m_enHidePosi = TOP;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, m_rect.left, 0, m_rect.right - m_rect.left, 2, SWP_NOCOPYBITS );

			m_rect.bottom -= m_rect.top;
			m_rect.top    =  0;
			
		} else if ( m_rect.left <=0 ){ 
			m_enHidePosi = LEFT;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, 0, m_rect.top, 2, m_rect.bottom - m_rect.top, SWP_NOCOPYBITS );

			m_rect.right -= m_rect.left;
			m_rect.left  = 0;
			
		} else if ( m_rect.right >= m_screenX ){ 
			m_enHidePosi = RIGHT;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, m_screenX-2, m_rect.top, 2, m_rect.bottom-m_rect.top, SWP_NOCOPYBITS );

			m_rect.left  = m_screenX - ( m_rect.right - m_rect.left );
			m_rect.right = m_screenX;//m_rect.left; //   m_rect.right - m_screenX
			
		} else {
			m_enHidePosi = NO;
		}	
	}
}



void CMyDockDlg::OnOK()
{
	

	//CDialogEx::OnOK();
}


void CMyDockDlg::OnCancel()
{
	

	CDialogEx::OnCancel();
}

BOOL CMyDockDlg::PreTranslateMessage(MSG* pMsg)
{

	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE ){
		//m_obj.SetFocus();
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMyDockDlg::OnBnClickedBnApp( UINT nCtlId )
{
	UINT nId = nCtlId - IDC_BN_HEAD;

	ShellExecute( NULL, _T("open") , m_vstrApp.at(nId), NULL, NULL, SW_SHOWNORMAL );
}
