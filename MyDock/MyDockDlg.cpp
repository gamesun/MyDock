
// MyDockDlg.cpp : 
//

#include "stdafx.h"
#include "MyDock.h"
#include "MyDockDlg.h"
#include "afxdialogex.h"
#include "AboutDlg.h"
#include "Vsstyle.h"
#include "Vssym32.h"
#include "LnkParser.h"
#include "UrlParser.h"
#include <atlbase.h>
#include <algorithm>

#include <dwmapi.h>
#pragma comment( lib, "Dwmapi.lib")
#pragma comment (lib, "UxTheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//static UINT WM_MY_MSG_REWRITE_APP_SETTING = RegisterWindowMessage(_T("ReWriteAppSetting"));


//using namespace std;

const int c_nIconSize[] = {
	16,		// 0
	24,		// 1
	32,		// 2
	48,		// 3
	64,		// 4
	72,		// 5
	96,		// 6
	128		// 7
};

CString strKey[8] = { "title", "link", "para", "workdir", "tip", "icon", "icoid", "isURL" };

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
	ON_CONTROL_RANGE(STN_CLICKED, IDC_STN_HEAD, IDC_STN_END, &CMyDockDlg::OnBnClickedBnApp)
	ON_CONTROL_RANGE(STN_CLICKED, IDC_STN_TITLE_HEAD, IDC_STN_TITLE_END, &CMyDockDlg::OnBnClickedBnAppTitle)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_RCLICKMENU_SETTING, &CMyDockDlg::OnRclickmenuSetting)
	ON_COMMAND(ID_RCLICKMENU_CLOSE, &CMyDockDlg::OnRclickmenuClose)
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_RCLICKMENU_SHOWTITLES, &CMyDockDlg::OnRclickmenuShowtitles)
	ON_WM_NCCALCSIZE()
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_RCLICKMENU_ABOUT, &CMyDockDlg::OnRclickmenuAbout)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_RCLICKTITLEMENU_UP, &CMyDockDlg::OnRclickTitleMenuUp)
	ON_COMMAND(ID_RCLICKTITLEMENU_DOWN, &CMyDockDlg::OnRclickTitleMenuDown)
//	ON_REGISTERED_MESSAGE(WM_MY_MSG_REWRITE_APP_SETTING, &CMyDockDlg::OnReWriteAppSetting)
ON_COMMAND(ID_RCLICKTITLEMENU_REMOVE, &CMyDockDlg::OnRclickTitleMenuRemove)
END_MESSAGE_MAP()


BOOL CMyDockDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 
	SetIcon(m_hIcon, FALSE);		// 


	//DWORD Style = ::GetWindowLong(AfxGetMainWnd()->m_hWnd, GWL_EXSTYLE);
 //   Style = WS_EX_TOOLWINDOW ;
 //   AfxGetMainWnd()->ShowWindow(SW_HIDE);
 //   ::SetWindowLong(AfxGetMainWnd()->m_hWnd, GWL_EXSTYLE, Style); 
 //   AfxGetMainWnd()->ShowWindow(SW_SHOW);
	m_bIsHiding = false;

	ModifyStyle( NULL, WS_THICKFRAME );

	ZeroMemory( &m_osvi, sizeof( OSVERSIONINFO ) );
	m_osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
	BOOL bRet = GetVersionEx( &m_osvi );
	if ( FALSE == bRet ){
		DWORD e = GetLastError();
		TRACE( "Error:%d", e );
		ASSERT( 0 );
	}
	DwmIsCompositionEnabled( &m_bIsAeroGlassEn );
	if ( m_osvi.dwMajorVersion >= 6 ){
		memset( &m_marginsAll, -1, sizeof(MARGINS) );
		memset( &m_marginsNone, 0, sizeof(MARGINS) );
		
		DwmExtendFrameIntoClientArea( m_hWnd, &m_marginsAll );
		SetBackgroundColor( RGB (200, 201, 202) );
    }

	//DWM_BLURBEHIND bb = {0};
 //   bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
 //   bb.fEnable = true;
 //   bb.hRgnBlur = NULL;
 //   DwmEnableBlurBehindWindow(m_hWnd, &bb);

	SetWindowPos( &wndTopMost, NULL, NULL, APP_WIDTH, APP_HEIGHT, SWP_NOMOVE );
	
//	m_hCursor = SetCursor( LoadCursor( 0, IDC_ARROW ) );

	m_screenX = GetSystemMetrics( SM_CXSCREEN );
	m_screenY = GetSystemMetrics( SM_CYSCREEN );
	//m_dwLastActiveTime = GetTickCount();
	m_enHidePosi = NO;
	SetTimer( TIMER_EVENT_ID_100MS, 100, NULL );

	m_bIsDraging = false;
	m_nTitleWidthMax = 0;
	m_sizeMain.SetSize( APP_WIDTH, APP_HEIGHT );
	m_sizeIcon.SetSize( 16, 16 );
	m_dwHoldTimeBeforeShow = 0;
	m_dwHoldTimeBeforeHide = 0;

	m_nClickedTitleIdx = -1;

	OleInitialize( NULL );

	LoadSetting();

	m_dwLastActiveTime = GetTickCount();
	DockedShow();

	return TRUE;  
}

CMyDockDlg::~CMyDockDlg(){
	
	SaveSetting();

	for (std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
		if ( i->pStnIcon ){
			delete i->pStnIcon;
		}

		if ( i->hIcon ){
			DestroyIcon( i->hIcon );
		}

		if ( i->pTipCtl ){
			delete i->pTipCtl;
		}

		if ( i->pStnTitle ){
			delete i->pStnTitle;
		}

		if ( i->pStnFont ){
			delete i->pStnFont;
		}
	}
}

void CMyDockDlg::LoadSetting( void ){
	CString strBuff;
	CString strSection;
	int nScreenX, nScreenY;

	GetModuleFileName( NULL, m_strSettingFile.GetBuffer(MAX_PATH) , MAX_PATH );
	m_strSettingFile.ReleaseBuffer();
	m_strSettingFile = m_strSettingFile.Left( m_strSettingFile.ReverseFind('\\') ) + "\\setting.ini";
	TRACE( "%s\n", m_strSettingFile );

	CFileStatus s;
	if ( !CFile::GetStatus( m_strSettingFile, s ) ){
		CString strErrMsg;
		strErrMsg.Format( "%s not found!", m_strSettingFile );
		MessageBox( strErrMsg );
		SendMessage( WM_CLOSE );
		return;
	}

	CString strDefaultBrowser;
	CRegKey rk;
	if ( rk.Open( HKEY_CLASSES_ROOT, "http\\shell\\open\\command", KEY_READ ) != ERROR_SUCCESS ){
		TRACE( "failed opening the HKEY_CLASSES_ROOT\\http\\shell\\open\\command\n" );
	} else {
		ULONG nLen;
		rk.QueryStringValue( "", strDefaultBrowser.GetBuffer(MAX_PATH), &nLen );
		strDefaultBrowser.ReleaseBuffer();
		strDefaultBrowser.Remove( '\"' );
		strDefaultBrowser = strDefaultBrowser.Left( strDefaultBrowser.Find( ".exe" ) + 4 );

		UINT nRet = -1;
		if ( ! strDefaultBrowser.IsEmpty() ){
			if ( 0 < ExtractIconEx( strDefaultBrowser, -1, NULL, NULL, 0 ) ){
				nRet = ExtractIconEx( strDefaultBrowser, 0, NULL, &m_hUrlIcon, 1 );
			}
		}
		if ( -1 == nRet ){
			nRet = ExtractIconEx( "imageres.dll", 11, NULL, &m_hUrlIcon, 1 );		// unknown executable file's icon.
			if ( nRet == -1 ){
				ASSERT(0);
			}
		}
	}

	nScreenX = GetPrivateProfileInt( "options", "screen x", 0, m_strSettingFile );

	nScreenY = GetPrivateProfileInt( "options", "screen y", 0, m_strSettingFile );

	UINT nTmp = GetPrivateProfileInt( "options", "icon size", 0, m_strSettingFile );
	nTmp = ( nTmp < sizeof( c_nIconSize ) / sizeof( c_nIconSize[0] ) ) ? nTmp : 0;
	m_sizeIcon.SetSize( c_nIconSize[nTmp], c_nIconSize[nTmp] );

	m_dwHoldTimeBeforeShow = GetPrivateProfileInt( "options", "hold time before show", 0, m_strSettingFile );
	
	m_dwHoldTimeBeforeHide = GetPrivateProfileInt( "options", "hold time before hide", 0, m_strSettingFile );

	GetPrivateProfileString( "options", "show titles", "no", strBuff.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
	strBuff.ReleaseBuffer();
	m_bIsShowTitle = ( "yes" == strBuff ) ? true : false;

	GetPrivateProfileString( "options", "font name", "SimSun", m_strFontName.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
	m_strFontName.ReleaseBuffer();
	Utf8ToAnsi( m_strFontName );

	m_nFontSize = GetPrivateProfileInt( "options", "font size", 12, m_strSettingFile );


	for ( UINT i = 0; i < MAX_APP_NUM; i++ ){
		ST_APP_INFO stAppInfo;
		stAppInfo.pStnIcon = NULL;
		stAppInfo.pStnTitle = NULL;
		stAppInfo.pStnFont = NULL;
		stAppInfo.hIcon = NULL;
		stAppInfo.nIcoId = 0;
		stAppInfo.pTipCtl = NULL;
		stAppInfo.bIsUrl = false;

		strSection.Format( "App%d", i + 1  );
		stAppInfo.nSortIdx = i + 1;

		GetPrivateProfileString( strSection, strKey[0], "", stAppInfo.strTitle.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strTitle.ReleaseBuffer();
		Utf8ToAnsi( stAppInfo.strTitle );
		stAppInfo.strTitle = " " + stAppInfo.strTitle;

		GetPrivateProfileString( strSection, strKey[1], "", stAppInfo.strLink.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strLink.ReleaseBuffer();
		if ( stAppInfo.strLink.IsEmpty() ){
			continue;
		}
		Utf8ToAnsi( stAppInfo.strLink );

		GetPrivateProfileString( strSection, strKey[2], "", stAppInfo.strPara.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strPara.ReleaseBuffer();
		Utf8ToAnsi( stAppInfo.strPara );

		GetPrivateProfileString( strSection, strKey[3], "", stAppInfo.strDir.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strDir.ReleaseBuffer();
		Utf8ToAnsi( stAppInfo.strDir );

		GetPrivateProfileString( strSection, strKey[4], "", stAppInfo.strTip.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strTip.ReleaseBuffer();
		Utf8ToAnsi( stAppInfo.strTip );

		GetPrivateProfileString( strSection, strKey[5], "", stAppInfo.strIcon.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strIcon.ReleaseBuffer();
		Utf8ToAnsi( stAppInfo.strIcon );

		stAppInfo.nIcoId = GetPrivateProfileInt( strSection, strKey[6], 0, m_strSettingFile );

		GetPrivateProfileString( strSection, strKey[7], "no", strBuff.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		strBuff.ReleaseBuffer();
		stAppInfo.bIsUrl = ( "yes" == strBuff ) ? true : false;

		m_vstAppInfo.push_back( stAppInfo );
		TRACE( "%s:%s\n", strKey[0], stAppInfo.strLink );
	}

	for ( UINT i = 0; i < m_vstAppInfo.size(); i++ ){
		CreateAppItem( i );
	}
	
	::SetClassLongPtr( m_vstAppInfo.at(0).pStnIcon->m_hWnd, GCL_HCURSOR, (LONG)LoadCursor( NULL, IDC_HAND ) );

	m_nTitleWidthMax = std::max_element( m_vstAppInfo.begin(), m_vstAppInfo.end(), AppStnWidthCmp )->rectTitle.Width();
	m_sizeMain.cy = APP_STN_TOP + APP_STN_H_DISTANCE * m_vstAppInfo.size() + APP_STN_BOTTOM;

	UpdateUI();

	SetWindowPos( &wndTopMost, nScreenX, nScreenY, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE );
}

void CMyDockDlg::CreateAppItem( int nIdx )
{
	ST_APP_INFO& stApp = m_vstAppInfo.at( nIdx );
	int l,t,r,b;
	l = APP_STN_W_SPACING;
	t = APP_STN_TOP + nIdx * APP_STN_H_DISTANCE;
	r = APP_STN_W_DISTANCE;
	b = t + m_sizeIcon.cy;

	if ( ! stApp.strLink.IsEmpty() ){
		//GetEnvironmentVariable();
		UINT nRet = 0;
		HICON hIcon16;	// 16 pixels 
		HICON hIcon32;	// 32 pixels
		if ( stApp.bIsUrl ){
			stApp.hIcon = m_hUrlIcon;
		} else if ( stApp.strIcon.IsEmpty() ){
			DWORD dwAttrib = GetFileAttributes( stApp.strLink );
			if ( ( dwAttrib != -1 ) && 
				( dwAttrib & FILE_ATTRIBUTE_DIRECTORY ) ){ 
				nRet = ExtractIconEx( "Shell32.dll", 4, &hIcon32, &hIcon16, 1 );		// opened folder icon
			} else if ( 0 < ExtractIconEx( stApp.strLink, -1, NULL, NULL, 0 ) ){
				nRet = ExtractIconEx( stApp.strLink, 0, &hIcon32, &hIcon16, 1 );
			} else {
				nRet = -1;
			}
		} else {
			UINT IconNum = ExtractIconEx( stApp.strIcon, -1, NULL, NULL, 0 );
			UINT nIcoId = ( stApp.nIcoId < IconNum ) ? stApp.nIcoId : 0;
			nRet = ExtractIconEx( stApp.strIcon, nIcoId, &hIcon32, &hIcon16, 1 );
		}
		if ( -1 == nRet ){
			nRet = ExtractIconEx( "imageres.dll", 11, &hIcon32, &hIcon16, 1 );		// unknown executable file's icon.
			if ( nRet == -1 ){
				ASSERT(0);
			}
		}

		if ( m_sizeIcon.cx <= 16 ){
			stApp.hIcon = hIcon16;
		} else {
			stApp.hIcon = hIcon32;
		}
		
		if ( NULL == stApp.hIcon ){
			ASSERT(0);
		} else {
			stApp.pStnIcon = new CTransparentImage;
			stApp.pStnIcon->Create( "", WS_CHILD | WS_VISIBLE | SS_ICON | SS_NOTIFY, CRect( l, t, r, b ), this, IDC_STN_HEAD + nIdx );
			//	stApp.pStnIcon->SetIcon( stApp.hIcon, 16, 16 );
			stApp.pStnIcon->SetIcon( stApp.hIcon, m_sizeIcon.cx, m_sizeIcon.cy );	// TODO: get the corresponding size icon

			stApp.pTipCtl = new CToolTipCtrl;
			stApp.pTipCtl->Create( this );
			stApp.pTipCtl->AddTool( GetDlgItem( IDC_STN_HEAD + nIdx ), ( stApp.strTip.IsEmpty() ) ? stApp.strTitle : stApp.strTip );

		}
	}

	if ( ! stApp.strTitle.IsEmpty() ){
		stApp.pStnTitle = new CStatic;
		stApp.rectTitle.SetRect( r + APP_TITLE_W_SPACING, t - APP_TITLE_H_OFFSET, r + 100, b + APP_TITLE_H_OFFSET );
		stApp.pStnTitle->Create( ""/*stApp.strTitle*/, WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOTIFY, stApp.rectTitle, this, IDC_STN_TITLE_HEAD + nIdx );

		stApp.pStnFont = new CFont;
		stApp.pStnFont->CreateFont( m_nFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH&FF_SWISS, m_strFontName );

		stApp.pStnTitle->SetFont( stApp.pStnFont );

		//	CRect rect;
		CSize size( 0, 0 );
		//	stApp.pStnTitle->GetWindowRect( rect );
		//	ScreenToClient( &rect );
		CClientDC dc( stApp.pStnTitle );
		dc.SelectObject( stApp.pStnFont );
		//	CFont *pOldFont = dc.SelectObject( this->GetFont() );	// this step is necessary, I used to think so.
		//CString str;
		//stApp.pStnTitle->GetWindowText( str );
		if( ::GetTextExtentPoint32( (HDC)dc, stApp.strTitle, stApp.strTitle.GetLength(), &size ) ){
			stApp.rectTitle.right = stApp.rectTitle.left + size.cx;
		//	stApp.rectTitle.bottom = stApp.rectTitle.top + size.cy;
		} else {
			TRACE( "GetTextExtentPoint32 fail to get the size of text!\n" );
		}
		stApp.pStnTitle->MoveWindow( stApp.rectTitle );
		//	dc.SelectObject( pOldFont );

	//	stApp.rectTitle.top -= 4;
	//	stApp.rectTitle.bottom += 4;
		stApp.rectTitle.right += 5;
		//CFont* pFont = stApp.pStnTitle->GetFont();
		//LOGFONT LogFont;
		//pFont->GetLogFont(&LogFont);
		//LogFont.lfUnderline = 1;
		//LogFont.lfHeight = 12;
		//pFont->Detach();
		//pFont->CreateFontIndirect(&LogFont);
		//stApp.pStnTitle->SetFont( pFont );
		//pFont->Detach();
	}
}


void CMyDockDlg::SaveSetting( void )
{
	char szBuff[16];

	_itoa_s( m_rectMain.left, szBuff, 10 );
	WritePrivateProfileString( "options", "screen x", szBuff, m_strSettingFile );

	_itoa_s( m_rectMain.top, szBuff, 10 );
	WritePrivateProfileString( "options", "screen y", szBuff, m_strSettingFile );

	WritePrivateProfileString( "options", "show titles", ( m_bIsShowTitle ? "yes" : "no" ), m_strSettingFile );
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
		//CClientDC pDC(this);
		//CRect m_rcClient, rectangle;  
		//this->GetClientRect(&m_rcClient);
		//int nWidth  = m_rcClient.Width();
		//int nHeight = m_rcClient.Height();	
		//for(int i=0; i<nWidth; i++)
		//{
		//	rectangle.SetRect(i, 0, i+1, nHeight);
		//	//dc.ExcludeClipRect(&rect)
		//	pDC.FillSolidRect(&rectangle, RGB(MulDiv(i, 32, nWidth)+30, MulDiv(i, 32, nWidth)+30, MulDiv(i, 32, nWidth)+30));
		//}
		
		CDialogEx::OnPaint();

		if ( m_osvi.dwMajorVersion >= 6 ){
			CRect rect;
			GetClientRect( &rect );

			DwmExtendFrameIntoClientArea( m_hWnd, &m_marginsAll );

			if ( m_bIsHiding ){
				CBrush brush( RGB( 255, 255, 255 ) );
				GetDC()->FillRect( &rect, &brush );
			} else {
				CBrush brush( RGB( 0, 0, 0 ) );
				GetDC()->FillRect( &rect, &brush );
			}
		}

		if ( m_bIsShowTitle ){
			CWindowDC dc(this);
			USES_CONVERSION;
			for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
				DrawGlowingText( dc.m_hDC, A2W( i->strTitle ), i->rectTitle );
			}
		}
	}
}

HCURSOR CMyDockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HBRUSH CMyDockDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	//if ( nCtlColor == CTLCOLOR_STATIC &&
	//	( IDC_STN_TITLE_HEAD <= pWnd->GetDlgCtrlID() ) &&
	//	( pWnd->GetDlgCtrlID() <= IDC_STN_TITLE_END ) ){

	//	CFont * cFont = new CFont;
	//	cFont->CreateFont( 12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
	//		ANSI_CHARSET, OUT_DEFAULT_PRECIS,
	//		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	//		DEFAULT_PITCH&FF_SWISS, "SimSun" );
	////	
	////	pDC->SetBkMode(TRANSPARENT);
	////	pDC->SetTextColor(RGB(255,255,0));
	//	pWnd->SetFont(cFont);
	////	HBRUSH B = CreateSolidBrush(RGB(125,125,255)); 
	////	return (HBRUSH) B;
	////case CTLCOLOR_LISTBOX:
	////	pDC->SetTextColor(RGB(0,0,0));
	////	pDC->SetBkColor(RGB(150,175,250));
	////	return (HBRUSH) (m_pBrush->GetSafeHandle());
	////case CTLCOLOR_EDIT:
	////	pDC->SetTextColor(RGB(0,0,0));
	////	pDC->SetBkColor(RGB(150,175,250));
	////	return (HBRUSH) (m_pBrush->GetSafeHandle());
	////case CTLCOLOR_DLG:
	////	m_brush.CreateSolidBrush( RGB( 200, 200, 200 ) );
	////	return (HBRUSH)m_brush.GetSafeHandle();
	//}
	if ( nCtlColor == CTLCOLOR_STATIC ){
		pDC->SetTextColor( RGB( 255, 255, 255 ) );
		pDC->SetBkMode( TRANSPARENT );
		return (HBRUSH)::GetStockObject( NULL_BRUSH );
	}
	return hbr;
}

BOOL CMyDockDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
//	SetCursor( m_hCursor );
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}

void CMyDockDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	m_dwLastActiveTime = GetTickCount();
	if ( m_bIsDraging ){
		PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM( point.x, point.y ) );
	}

	//CRect rect;
	//GetDlgItem( IDC_STN_TITLE_HEAD )->GetWindowRect( &rect );
	//ScreenToClient( &rect );

	//TRACE( "%d,%d\n" , point.x, point.y );
	//if ( rect.PtInRect( point ) ){
	//	//::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	//	m_hCursor = SetCursor( LoadCursor(0, IDC_HAND) );
	//} else {
	//	//::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	//	m_hCursor = SetCursor( LoadCursor(0, IDC_ARROW) );
	//}

	CDialogEx::OnMouseMove(nFlags, point);
}


void CMyDockDlg::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	m_dwLastActiveTime = GetTickCount();
	m_bIsDraging = false;
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
	case TIMER_EVENT_ID_100MS:
		if ( GetTickCount() - m_dwLastActiveTime < m_dwHoldTimeBeforeHide ){
			//DockedShow();
			SetTimer( HOLD_TIME_BEFORE_SHOW_ID, m_dwHoldTimeBeforeShow, NULL );
		} else {
			DockedHidden();
		}
		break;
	case HOLD_TIME_BEFORE_SHOW_ID:
		KillTimer( HOLD_TIME_BEFORE_SHOW_ID );
		if ( IsMouseInWindow() ){
			DockedShow();
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}



void CMyDockDlg::DockedShow( void ){
//	static UINT i = 0;
//	TRACE( "\nEnter DockedShow %d\n", i  );
	if( m_enHidePosi != NO && IsMouseInWindow() ){

		ModifyStyle( NULL, WS_THICKFRAME );	// Aero effect need the WS_THICKFRAME style.
		int seq = 0;
		switch( m_enHidePosi ){
		case TOP:
			m_bIsHiding = false;
			while ( ++seq <= SEQ_NUM ){
				this->SetWindowPos( NULL, m_rectMain.left, m_rectMain.top, m_rectMain.Width(), m_rectMain.Height() * seq / SEQ_NUM, SWP_SHOWWINDOW );
				if ( m_osvi.dwMajorVersion >= 6 ){
					CRect rect;
					GetClientRect( &rect );

					CBrush brush( RGB( 0, 0, 0 ) );
					GetDC()->FillRect( &rect, &brush );
				}
				Sleep( EVERY_TIME );
			}
			break;
		case LEFT:
			m_bIsHiding = false;
			while ( ++seq <= SEQ_NUM ){
				this->SetWindowPos( NULL, 0, 0, m_rectMain.Width() * seq / SEQ_NUM, m_rectMain.Height(), SWP_NOMOVE );
				if ( m_osvi.dwMajorVersion >= 6 ){
					CRect rect;
					GetClientRect( &rect );

					CBrush brush( RGB( 0, 0, 0 ) );
					GetDC()->FillRect( &rect, &brush );
				}

				Sleep( EVERY_TIME );
			}
			break;
		case RIGHT:
			m_bIsHiding = false;
			while ( ++seq <= SEQ_NUM ){
				this->SetWindowPos( NULL, m_rectMain.left + m_rectMain.Width() * ( SEQ_NUM - seq ) / SEQ_NUM, m_rectMain.top, m_rectMain.Width(), m_rectMain.Height(), SWP_SHOWWINDOW );
				if ( m_osvi.dwMajorVersion >= 6 ){
					CRect rect;
					GetClientRect( &rect );

					CBrush brush( RGB( 0, 0, 0 ) );
					GetDC()->FillRect( &rect, &brush );
				}
				Sleep( EVERY_TIME );
			}
			break;
		}
		m_enHidePosi = NO;
	}
//	TRACE( "\nLeave DockedShow %d\n", i++ );
}

void CMyDockDlg::DockedHidden( bool bIsForceHide ){
	CRect rect;
	GetWindowRect( &rect );
	
	if ( ( m_enHidePosi == NO && !IsMouseInWindow() ) || 
		( bIsForceHide ) ){
		m_rectMain = rect;
		if ( m_rectMain.top <=0 ){
			m_enHidePosi = TOP;
			ModifyStyle( WS_THICKFRAME, NULL );
			this->SetWindowPos( NULL, m_rectMain.left, 0, m_rectMain.Width(), 2, SWP_NOCOPYBITS );

		//	m_rectMain.bottom -= m_rectMain.top;
			m_rectMain.bottom = m_sizeMain.cy;
			m_rectMain.top    =  0;
			m_bIsHiding = true;

			if ( m_osvi.dwMajorVersion >= 6 ){
				DwmExtendFrameIntoClientArea( m_hWnd, &m_marginsNone );
			}
		} else if ( m_rectMain.left <=0 ){
			m_enHidePosi = LEFT;
			ModifyStyle( WS_THICKFRAME, NULL );
			this->SetWindowPos( NULL, 0, m_rectMain.top, 2, m_rectMain.Height(), SWP_NOCOPYBITS );

		//	m_rectMain.right -= m_rectMain.left;
			m_rectMain.right = m_sizeMain.cx;
			m_rectMain.left  = 0;
			m_bIsHiding = true;

			if ( m_osvi.dwMajorVersion >= 6 ){
				DwmExtendFrameIntoClientArea( m_hWnd, &m_marginsNone );
			}
		} else if ( m_rectMain.right >= m_screenX ){ 
			m_enHidePosi = RIGHT;
			ModifyStyle( WS_THICKFRAME, NULL );
			this->SetWindowPos( NULL, m_screenX-2, m_rectMain.top, 2, m_rectMain.Height(), SWP_NOCOPYBITS );

			m_rectMain.left  = m_screenX - m_rectMain.Width();
			m_rectMain.right = m_screenX;//m_rectMain.left; //   m_rectMain.right - m_screenX
			m_bIsHiding = true;

			if ( m_osvi.dwMajorVersion >= 6 ){
				DwmExtendFrameIntoClientArea( m_hWnd, &m_marginsNone );
			}
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

//static bool dbgFlg = false;
BOOL CMyDockDlg::PreTranslateMessage(MSG* pMsg)
{
	//if ( dbgFlg && pMsg->message == WM_PAINT ){
	//	return TRUE;
	//}
//	( pMsg->message == WM_TIMER ) ? 0 : TRACE( ",%04X", pMsg->message );

	switch ( pMsg->message ){
	case WM_KEYDOWN:
		if ( pMsg->wParam == VK_ESCAPE ){
			return TRUE;
		}
		break;
	case WM_RBUTTONDOWN:
		RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW );
		break;
	case WM_RBUTTONUP:
		CPoint pt;
		::GetCursorPos( &pt );
		ScreenToClient( &pt );
		for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
			if ( i->rectTitle.PtInRect( pt ) ){
				RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW );
				CMenu dMenu;
				if ( !dMenu.LoadMenu( IDR_MENU1 ) )
					AfxThrowResourceException();
				CMenu* pPopMenu = dMenu.GetSubMenu( 1 );
				if ( pPopMenu != NULL ){
					m_nClickedTitleIdx = distance( m_vstAppInfo.begin(), i );
					pPopMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, pMsg->pt.x, pMsg->pt.y, this );
				}
				
				return TRUE;
			}
		}
		break;
	}

	if ( ! m_bIsShowTitle ){
		for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
			if ( i->pTipCtl ){
				i->pTipCtl->RelayEvent(pMsg);
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMyDockDlg::OnBnClickedBnApp( UINT nCtlId )
{
	UINT nId = nCtlId - IDC_STN_HEAD;
	ST_APP_INFO &info = m_vstAppInfo[nId];
	Invalidate( FALSE );
	HINSTANCE h = ShellExecute( NULL, "open", info.strLink, info.strPara, info.strDir, SW_SHOW );
	//switch ( (long)h ){
	//case ERROR_FILE_NOT_FOUND:
	//	
	//	break;
	//}
	DockedHidden( true );
}

void CMyDockDlg::OnBnClickedBnAppTitle( UINT nCtlId )
{
	OnBnClickedBnApp( nCtlId - IDC_STN_TITLE_HEAD + IDC_STN_HEAD );
}

void CMyDockDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bIsDraging = true;
	Invalidate( FALSE );
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMyDockDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bIsDraging = false;
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CMyDockDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	CPoint pt;
	CMenu dMenu;
	if ( !dMenu.LoadMenu( IDR_MENU1 ) )
		AfxThrowResourceException();
	CMenu* pPopMenu = dMenu.GetSubMenu( 0 );
	if ( pPopMenu != NULL ){
		pPopMenu->CheckMenuItem( ID_RCLICKMENU_SHOWTITLES, ( m_bIsShowTitle ? MF_CHECKED : MF_UNCHECKED ) );
		::GetCursorPos( &pt );
		pPopMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this );
	}

	CDialogEx::OnRButtonUp( nFlags, point );
}


void CMyDockDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	Invalidate( FALSE );
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CMyDockDlg::OnRclickmenuSetting()
{
	ShellExecute( NULL, "open", m_strSettingFile, NULL, NULL, SW_SHOW );
}


void CMyDockDlg::OnRclickmenuClose()
{
	EndDialog( 0 );
}


void CMyDockDlg::OnRclickmenuShowtitles()
{
	m_bIsShowTitle = !m_bIsShowTitle;

	UpdateUI();
}

void CMyDockDlg::UpdateUI( void )
{
	if ( m_bIsShowTitle ){
		for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
			if ( i->pStnTitle ){
				i->pStnTitle->ModifyStyle( 0, WS_VISIBLE );
			}
		}

		m_sizeMain.cx = m_nTitleWidthMax + APP_STN_W_SPACING + m_sizeIcon.cx + APP_TITLE_W_SPACING + APP_STN_W_SPACING + 10;
	} else {
		for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
			if ( i->pStnTitle ){
				i->pStnTitle->ModifyStyle( WS_VISIBLE, 0 );
			}
		}

		m_sizeMain.cx = APP_STN_W_SPACING + m_sizeIcon.cx + APP_STN_W_SPACING;
	}

	m_rectMain.right  = m_rectMain.left + m_sizeMain.cx;
	m_rectMain.bottom = m_rectMain.top  + m_sizeMain.cy;

	ModifyStyle( NULL, WS_THICKFRAME );
	SetWindowPos( &wndTopMost, 0, 0, m_rectMain.Width(), m_rectMain.Height(), SWP_SHOWWINDOW | SWP_NOMOVE );
	m_bIsHiding  = false;
	m_enHidePosi = NO;

//	Invalidate( FALSE );	// do not erase the background
	RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW );
}


void CMyDockDlg::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
//	CDialogEx::OnNcCalcSize(bCalcValidRects, lpncsp);		// comment here, to make the border = 0
}


void CMyDockDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	//switch ( nState ){
	//case WA_INACTIVE:
	//	Invalidate( FALSE );
	//	break;
	//case WA_ACTIVE:
	//	Invalidate( FALSE );
	//	break;
	//case WA_CLICKACTIVE:
	//	Invalidate( FALSE );
	//	break;
	//default:
	//	break;
	//}
	
	Invalidate( FALSE );
}


void CMyDockDlg::OnRclickmenuAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


void CMyDockDlg::DrawGlowingText( HDC hDC, LPWSTR szText, RECT &rcArea, 
	DWORD dwTextFlags, int iGlowSize )
{
	// get the handle of the theme
	HTHEME hThm = OpenThemeData( /*GetDesktopWindow()*/m_hWnd, L"TextStyle"/*L"Window"*/ );
	
	// create DIB
	HDC hMemDC = CreateCompatibleDC( hDC );
	BITMAPINFO bmpinfo = { 0 };
	bmpinfo.bmiHeader.biSize = sizeof( bmpinfo.bmiHeader );
	bmpinfo.bmiHeader.biBitCount = 32;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biWidth = rcArea.right - rcArea.left;
	bmpinfo.bmiHeader.biHeight = -( rcArea.bottom - rcArea.top );
	HBITMAP hBmp = CreateDIBSection( hMemDC, &bmpinfo, DIB_RGB_COLORS, 0, NULL, 0 );
	if ( hBmp == NULL ) return;
	HGDIOBJ hBmpOld = SelectObject( hMemDC, hBmp );
	
	// draw options
	DTTOPTS dttopts = { 0 };
	dttopts.dwSize = sizeof( DTTOPTS );
	dttopts.dwFlags = DTT_GLOWSIZE | DTT_COMPOSITED | DTT_SHADOWTYPE;
	dttopts.iGlowSize = iGlowSize;	// the size of the glow
	dttopts.iTextShadowType = /*TST_NONE TST_SINGLE*/ TST_CONTINUOUS;
	
	// Select afont.
	LOGFONT lgFont={0};
	HFONT hFontOld = NULL;

	HFONT hFont =  CreateFont( m_nFontSize, 0, 0, 0, 400,
		FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH,
		m_strFontName );

	hFontOld = (HFONT)SelectObject( hMemDC, hFont );


	// draw the text
	RECT rc = { 0, 0, rcArea.right - rcArea.left, rcArea.bottom - rcArea.top };
	HRESULT hr = DrawThemeTextEx( hThm, hMemDC, 0/*TEXT_LABEL*/, 0, szText, -1, dwTextFlags , &rc, &dttopts );
	if ( FAILED( hr ) ) return;
	BitBlt( hDC, rcArea.left, rcArea.top, rcArea.right - rcArea.left, 
		rcArea.bottom - rcArea.top, hMemDC, 0, 0, SRCCOPY | CAPTUREBLT );
	
	//Clear
	SelectObject( hMemDC, hBmpOld );
	DeleteObject( hBmp );
	DeleteDC( hMemDC );
	CloseThemeData( hThm );
}

void CMyDockDlg::OnDropFiles(HDROP hDropInfo)
{
	CString strFile;
	int nValidLnkCnt;
	char * lpszFileName = new char[MAX_PATH];

	int nFileCount = ::DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, MAX_PATH );

	nValidLnkCnt = 0;
	CString strTmp;
	for ( int i = 0; i < nFileCount; i++ ){
		/*UINT nChars = */::DragQueryFile( hDropInfo, i, strFile.GetBuffer(MAX_PATH), MAX_PATH );
		strFile.ReleaseBuffer();
		TRACE( "%s\n", strFile );
		strTmp = strFile;
		strTmp.MakeLower();
		if ( "lnk" == strTmp.Right( 3 ) ){
			if ( SUCCEEDED( GetLnkInfo( strFile ) ) ){
				nValidLnkCnt++;
			}
		} else if ( "url" == strTmp.Right( 3 ) ){
			if ( SUCCEEDED( GetUrlInfo( strFile ) ) ){
				nValidLnkCnt++;
			}			
		}
	}
	UpdateUI();

	for ( int i = nValidLnkCnt; i >= 1; i-- ){
		SaveAppSetting( m_vstAppInfo.size() - i );
	}
	

	::DragFinish( hDropInfo );
	delete[] lpszFileName;

	CDialogEx::OnDropFiles(hDropInfo);
}


HRESULT CMyDockDlg::GetLnkInfo( const CString& strDest )
{
	ST_APP_INFO stAppInfo;
	CHAR szBuff[MAX_PATH];
	CLnkParser lp;

	if ( FAILED( lp.LoadFile( strDest.AllocSysString() ) ) ){
		return -1;
	}

	if ( SUCCEEDED( lp.GetTargetPath( szBuff, MAX_PATH ) ) ){
		stAppInfo.strLink.Format( "%s", szBuff );
		TRACE( "Path\t%s\n", szBuff );
	}

	if ( SUCCEEDED( lp.GetTargetArguments( szBuff, MAX_PATH ) ) ){
		stAppInfo.strPara.Format( "%s", szBuff );
		TRACE( "Args\t%s\n", szBuff );
	}

	if ( SUCCEEDED( lp.GetTargetWorkingDirectory( szBuff, MAX_PATH ) ) ){
		stAppInfo.strDir.Format( "%s", szBuff );
		TRACE( "Dir \t%s\n", szBuff );
	}

	if ( SUCCEEDED( lp.GetTargetIconLocation( szBuff, MAX_PATH, (int*)&stAppInfo.nIcoId ) ) ){
		stAppInfo.strIcon.Format( "%s", szBuff );
		TRACE( "Icon\t%s,%d\n", szBuff, stAppInfo.nIcoId );
	}

	stAppInfo.pTipCtl = NULL;
	stAppInfo.strTitle = strDest.Mid( strDest.ReverseFind( '\\' ) + 1 );
	stAppInfo.strTitle.Replace( ".lnk", "" );
	stAppInfo.strTitle = " " + stAppInfo.strTitle;
	stAppInfo.pStnIcon  = NULL;
	stAppInfo.pStnTitle = NULL;
	stAppInfo.pStnFont = NULL;
	stAppInfo.bIsUrl = false;
	stAppInfo.nSortIdx = m_vstAppInfo.size();

	m_vstAppInfo.push_back( stAppInfo );
	
	m_sizeMain.cy = APP_STN_TOP + APP_STN_H_DISTANCE * m_vstAppInfo.size() + APP_STN_BOTTOM;

	CreateAppItem( m_vstAppInfo.size() - 1 );

	return S_OK;
}

void CMyDockDlg::SaveAppSetting( UINT nIdx )
{
	ST_APP_INFO& stApp = m_vstAppInfo.at( nIdx );
	CString strSection;
	char szBuff[16];
	char* pszBuff;

	strSection.Format( "App%d", nIdx + 1 );

	// avoid the space at begin.
//	WritePrivateProfileString( strSection, strKey[0], stApp.strTitle.Mid( 1 ), m_strSettingFile );	
	CodePageConvert( CP_ACP, stApp.strTitle.Mid( 1 ), stApp.strTitle.GetLength() - 1, CP_UTF8, pszBuff );
	WritePrivateProfileString( strSection, strKey[0], pszBuff, m_strSettingFile );
	delete[] pszBuff;

	CodePageConvert( CP_ACP, stApp.strLink, stApp.strLink.GetLength(), CP_UTF8, pszBuff );
	WritePrivateProfileString( strSection, strKey[1], pszBuff, m_strSettingFile );
	delete[] pszBuff;

	CodePageConvert( CP_ACP, stApp.strPara, stApp.strPara.GetLength(), CP_UTF8, pszBuff );
	WritePrivateProfileString( strSection, strKey[2], pszBuff, m_strSettingFile );
	delete[] pszBuff;

	CodePageConvert( CP_ACP, stApp.strDir, stApp.strDir.GetLength(), CP_UTF8, pszBuff );
	WritePrivateProfileString( strSection, strKey[3], pszBuff, m_strSettingFile );
	delete[] pszBuff;

	CodePageConvert( CP_ACP, stApp.strTip, stApp.strTip.GetLength(), CP_UTF8, pszBuff );
	WritePrivateProfileString( strSection, strKey[4], pszBuff, m_strSettingFile );
	delete[] pszBuff;

	CodePageConvert( CP_ACP, stApp.strIcon, stApp.strIcon.GetLength(), CP_UTF8, pszBuff );
	WritePrivateProfileString( strSection, strKey[5], pszBuff, m_strSettingFile );
	delete[] pszBuff;

	// UTF-8 is backwards compatible with ASCII (ANSI)
	_itoa_s( stApp.nIcoId, szBuff, 10 );
	WritePrivateProfileString( strSection, strKey[6], szBuff, m_strSettingFile );
	WritePrivateProfileString( strSection, strKey[7], ( stApp.bIsUrl ) ? "yes" : "no", m_strSettingFile );
}

HRESULT CMyDockDlg::GetUrlInfo( const CString& strDest )
{
	ST_APP_INFO stAppInfo;
	CHAR szBuff[MAX_PATH];
	CUrlParser up;

	up.LoadFile( strDest.AllocSysString() );

	up.GetTargetUrl( szBuff, MAX_PATH );
	stAppInfo.strLink.Format( "%s", szBuff );

	up.GetTargetIconLocation( szBuff, MAX_PATH, (int*)&stAppInfo.nIcoId );
	stAppInfo.strIcon.Format( "%s", szBuff );

	stAppInfo.pTipCtl = NULL;
	stAppInfo.strTitle = strDest.Mid( strDest.ReverseFind( '\\' ) + 1 );
	int npos = stAppInfo.strTitle.ReverseFind( '.' );
	if ( 0 <= npos ){
		stAppInfo.strTitle = stAppInfo.strTitle.Left( npos );
	}
	stAppInfo.strTitle = " " + stAppInfo.strTitle;
	stAppInfo.pStnIcon  = NULL;
	stAppInfo.pStnTitle = NULL;
	stAppInfo.pStnFont = NULL;
	stAppInfo.bIsUrl = true;
	stAppInfo.nSortIdx = m_vstAppInfo.size();

	m_vstAppInfo.push_back( stAppInfo );
	
	m_sizeMain.cy = APP_STN_TOP + APP_STN_H_DISTANCE * m_vstAppInfo.size() + APP_STN_BOTTOM;

	CreateAppItem( m_vstAppInfo.size() - 1 );

	return S_OK;
}

int Utf8ToAnsi( CString& strInOut )
{
	char *pszTmp;
	int nLen = CodePageConvert( CP_UTF8, strInOut, strInOut.GetLength(), CP_ACP, pszTmp );
	strInOut = pszTmp;
	delete[] pszTmp;
	
	return nLen;
}

int AnsiToUtf8( CString& strInOut )
{
	char *pszTmp;
	int nLen = CodePageConvert( CP_ACP, strInOut, strInOut.GetLength(), CP_UTF8, pszTmp );
	strInOut = pszTmp;
	delete[] pszTmp;

	return nLen;
}

int CodePageConvert( UINT SrcCodePage, LPCTSTR pszSrc, int iBuffLen, UINT DestCodePage, char* &pszDest )
{
	int iWideCharCnt = ::MultiByteToWideChar( SrcCodePage, 0, pszSrc, iBuffLen, NULL, 0 );
	LPWSTR lpszWideChar = new wchar_t[iWideCharCnt + 1];
	memset(lpszWideChar, 0, ( iWideCharCnt + 1 ) * sizeof( WCHAR ) );
	iWideCharCnt = MultiByteToWideChar( SrcCodePage, 0, pszSrc, iBuffLen, lpszWideChar, iWideCharCnt );

	if( DestCodePage == 54936 
		&& !IsValidCodePage( 54936 ) )
		DestCodePage = 936;

	int iDestCnt = WideCharToMultiByte( DestCodePage, 0, lpszWideChar, iWideCharCnt, NULL, 0, NULL, NULL );
	pszDest = new char[iDestCnt + 1];
	memset( pszDest, 0, iDestCnt + 1 );
	iDestCnt = WideCharToMultiByte( DestCodePage, 0, lpszWideChar, iWideCharCnt, pszDest, iDestCnt, NULL, NULL );

	delete []lpszWideChar; 
	return iDestCnt;
}


void CMyDockDlg::OnRclickTitleMenuUp()
{
	if ( ( 0 < m_nClickedTitleIdx ) &&						// when m_nClickedTitleIdx is 0, do nothing
		( m_nClickedTitleIdx < (int)m_vstAppInfo.size() ) ){
		SwapAppStn( m_nClickedTitleIdx - 1, m_nClickedTitleIdx );
		m_nClickedTitleIdx = -1;
	}
}


void CMyDockDlg::OnRclickTitleMenuDown()
{
	if ( ( 0 <= m_nClickedTitleIdx ) &&
		( m_nClickedTitleIdx < (int)m_vstAppInfo.size() - 1 ) ){	// when m_nClickedTitleIdx is m_vstAppInfo.size()-1, do nothing
		SwapAppStn( m_nClickedTitleIdx, m_nClickedTitleIdx + 1 );
		m_nClickedTitleIdx = -1;
	}
}

void CMyDockDlg::OnRclickTitleMenuRemove()
{
	if ( ( 0 <= m_nClickedTitleIdx ) &&
		( m_nClickedTitleIdx < (int)m_vstAppInfo.size() ) ){
		ST_APP_INFO &stApp = m_vstAppInfo[m_nClickedTitleIdx];
		delete stApp.pTipCtl;
		delete stApp.pStnIcon;
		delete stApp.pStnTitle;
		delete stApp.pStnFont;
		m_vstAppInfo.erase( m_vstAppInfo.begin() + m_nClickedTitleIdx );
		
		for ( std::vector<ST_APP_INFO>::iterator iter = m_vstAppInfo.begin() + m_nClickedTitleIdx; iter != m_vstAppInfo.end(); iter++ ){
			CRect rect;
			iter->pStnIcon->GetWindowRect( &rect );
			ScreenToClient( &rect );
			rect.OffsetRect( 0, -APP_STN_H_DISTANCE );
			iter->pStnIcon->MoveWindow( &rect );

			iter->rectTitle.OffsetRect( 0, -APP_STN_H_DISTANCE );
			iter->pStnTitle->MoveWindow( iter->rectTitle );
		}

		m_nTitleWidthMax = std::max_element( m_vstAppInfo.begin(), m_vstAppInfo.end(), AppStnWidthCmp )->rectTitle.Width();
		m_sizeMain.cy = APP_STN_TOP + APP_STN_H_DISTANCE * m_vstAppInfo.size() + APP_STN_BOTTOM;
		UpdateUI();

		ReWriteAppSetting();
		//CString strSection;
		//strSection.Format( "App%d", m_nClickedTitleIdx + 1 );
		//WritePrivateProfileString( strSection, NULL, NULL, m_strSettingFile );	// delete section

		m_nClickedTitleIdx = -1;
	}
}

bool AppStnWidthCmp( const ST_APP_INFO &a, const ST_APP_INFO &b )
{
	return a.rectTitle.Width() < b.rectTitle.Width();
}

void CMyDockDlg::SortAppStn( void )
{
	std::sort( m_vstAppInfo.begin(), m_vstAppInfo.end() );
}

void CMyDockDlg::SwapAppStn( UINT nIdxA, UINT nIdxB )
{
	if ( ( nIdxA == nIdxB ) || 
		( nIdxA >= m_vstAppInfo.size() ) ||
		( nIdxB >= m_vstAppInfo.size() ) ){
		return;
	}

	ST_APP_INFO &stAppA = m_vstAppInfo.at( nIdxA );
	ST_APP_INFO &stAppB = m_vstAppInfo.at( nIdxB );

	int dyA = ( nIdxA < nIdxB ) ? APP_STN_H_DISTANCE : -(APP_STN_H_DISTANCE);
	int dyB = -dyA;

	stAppA.rectTitle.OffsetRect( 0, dyA );
	stAppB.rectTitle.OffsetRect( 0, dyB );

	delete stAppA.pStnTitle;
	delete stAppB.pStnTitle;

	stAppA.pStnTitle = new CStatic;
	stAppB.pStnTitle = new CStatic;

	stAppA.pStnTitle->Create( "", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOTIFY, stAppA.rectTitle, this, IDC_STN_TITLE_HEAD + nIdxB );
	stAppB.pStnTitle->Create( "", WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOTIFY, stAppB.rectTitle, this, IDC_STN_TITLE_HEAD + nIdxA );

	stAppA.pStnTitle->SetFont( stAppA.pStnFont );
	stAppB.pStnTitle->SetFont( stAppB.pStnFont );


	CRect rectA, rectB;
	stAppA.pStnIcon->GetWindowRect( rectA );
	stAppB.pStnIcon->GetWindowRect( rectB );
	ScreenToClient( &rectA );
	ScreenToClient( &rectB );

	stAppA.pTipCtl->DelTool( GetDlgItem( IDC_STN_HEAD + nIdxA ) );
	stAppB.pTipCtl->DelTool( GetDlgItem( IDC_STN_HEAD + nIdxB ) );
	delete stAppA.pStnIcon;
	delete stAppB.pStnIcon;

	stAppA.pStnIcon = new CTransparentImage;
	stAppB.pStnIcon = new CTransparentImage;

	// swap position & ID
	stAppA.pStnIcon->Create( "", WS_CHILD | WS_VISIBLE | SS_ICON | SS_NOTIFY, rectB, this, IDC_STN_HEAD + nIdxB );
	stAppB.pStnIcon->Create( "", WS_CHILD | WS_VISIBLE | SS_ICON | SS_NOTIFY, rectA, this, IDC_STN_HEAD + nIdxA );
	stAppA.pStnIcon->SetIcon( stAppA.hIcon, m_sizeIcon.cx, m_sizeIcon.cy );
	stAppB.pStnIcon->SetIcon( stAppB.hIcon, m_sizeIcon.cx, m_sizeIcon.cy );
	
	stAppA.pTipCtl->AddTool( GetDlgItem( IDC_STN_HEAD + nIdxA ), ( stAppA.strTip.IsEmpty() ) ? stAppA.strTitle : stAppA.strTip );
	stAppB.pTipCtl->AddTool( GetDlgItem( IDC_STN_HEAD + nIdxB ), ( stAppB.strTip.IsEmpty() ) ? stAppB.strTitle : stAppB.strTip );
	
	std::swap( stAppA, stAppB );
	
	RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_UPDATENOW );

	//// re-write the App? in the setting.ini
	//CString strSectionA, strSectionB;
	//strSectionA.Format( "App%d", nIdxA + 1 );
	//strSectionB.Format( "App%d", nIdxB + 1 );
	//WritePrivateProfileString( strSectionA, NULL, NULL, m_strSettingFile );	// delete the section
	//WritePrivateProfileString( strSectionB, NULL, NULL, m_strSettingFile );	// delete the section

	//SaveAppSetting( nIdxA );
	//SaveAppSetting( nIdxB );

//	::SendMessage( m_hWnd, WM_MY_MSG_REWRITE_APP_SETTING, NULL, NULL );
	ReWriteAppSetting();
}

void CMyDockDlg::ReWriteAppSetting( void )
{
	for ( UINT i = 0; i < MAX_APP_NUM/*m_vstAppInfo.size()*/; i++ ){
		CString strSection;
		strSection.Format( "App%d", i + 1 );
		WritePrivateProfileString( strSection, NULL, NULL, m_strSettingFile );	// delete section
	}

	for ( UINT i = 0; i < m_vstAppInfo.size(); i++ ){
		SaveAppSetting( i );
	}

//	return S_OK;
}
