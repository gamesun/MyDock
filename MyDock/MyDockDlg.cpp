
// MyDockDlg.cpp : 
//

#include "stdafx.h"
#include "MyDock.h"
#include "MyDockDlg.h"
#include "afxdialogex.h"
#include "AboutDlg.h"
#include "Vsstyle.h"
#include "Vssym32.h"


#include <dwmapi.h>
#pragma comment( lib, "Dwmapi.lib")
#pragma comment (lib, "UxTheme.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
	m_nAppWidthTitle = 0;
	m_sizeApp.SetSize( APP_WIDTH, APP_HEIGHT );
	m_sizeIcon.SetSize( 16, 16 );
	m_dwHoldTimeBeforeShow = 0;
	m_dwHoldTimeBeforeHide = 0;

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
	CString strKey[7] = { "link", "para", "workdir", "tip", "icon", "icoid", "title" };
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

	GetPrivateProfileString( "options", "screen x", "0", strBuff.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
	strBuff.ReleaseBuffer();
	nScreenX = strtol( strBuff, NULL, 10 );

	GetPrivateProfileString( "options", "screen y", "0", strBuff.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
	strBuff.ReleaseBuffer();
	nScreenY = strtol( strBuff, NULL, 10 );

	GetPrivateProfileString( "options", "icon size", "0", strBuff.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
	strBuff.ReleaseBuffer();
	int nTmp = strtol( strBuff, NULL, 10 );
	if ( 0 <= nTmp && nTmp < sizeof( c_nIconSize ) / sizeof( c_nIconSize[0] ) ){
		m_sizeIcon.SetSize( c_nIconSize[nTmp], c_nIconSize[nTmp] );
	}

	GetPrivateProfileString( "options", "hold time before show", "0", strBuff.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
	strBuff.ReleaseBuffer();
	m_dwHoldTimeBeforeShow = strtol( strBuff, NULL, 10 );

	GetPrivateProfileString( "options", "hold time before hide", "0", strBuff.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
	strBuff.ReleaseBuffer();
	m_dwHoldTimeBeforeHide = strtol( strBuff, NULL, 10 );

	GetPrivateProfileString( "options", "show titles", "no", strBuff.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
	strBuff.ReleaseBuffer();
	m_bIsShowTitle = ( "yes" == strBuff ) ? true : false;

	GetPrivateProfileString( "options", "font name", "SimSun", m_strFontName.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
	m_strFontName.ReleaseBuffer();
	//if ( m_strFontName.IsEmpty() ){
	//	m_strFontName = "SimSun";
	//}

	GetPrivateProfileString( "options", "font size", "12", strBuff.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
	strBuff.ReleaseBuffer();
	m_nFontSize = strtol( strBuff, NULL, 10 );
	

	for ( UINT i = 0; i < MAX_APP_NUM; i++ ){
		ST_APP_INFO stAppInfo;
		stAppInfo.pStnIcon = NULL;
		stAppInfo.pStnTitle = NULL;
		stAppInfo.pStnFont = NULL;
		stAppInfo.hIcon = NULL;
		stAppInfo.nIcoId = 0;
		stAppInfo.pTipCtl = NULL;

		strSection.Format( "App%d", i + 1  );

		GetPrivateProfileString( strSection, strKey[0], "", stAppInfo.strLink.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strLink.ReleaseBuffer();

		GetPrivateProfileString( strSection, strKey[1], "", stAppInfo.strPara.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strPara.ReleaseBuffer();

		GetPrivateProfileString( strSection, strKey[2], "", stAppInfo.strDir.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strDir.ReleaseBuffer();

		GetPrivateProfileString( strSection, strKey[3], "", stAppInfo.strTip.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strTip.ReleaseBuffer();

		GetPrivateProfileString( strSection, strKey[4], "", stAppInfo.strIcon.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strIcon.ReleaseBuffer();

		GetPrivateProfileString( strSection, strKey[5], "", strBuff.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		strBuff.ReleaseBuffer();
		stAppInfo.nIcoId = strtol( strBuff, NULL, 10 );

		GetPrivateProfileString( strSection, strKey[6], "", stAppInfo.strTitle.GetBuffer(MAX_PATH), MAX_PATH, m_strSettingFile );
		stAppInfo.strTitle.ReleaseBuffer();
		stAppInfo.strTitle = " " + stAppInfo.strTitle;

		int n = MultiByteToWideChar( CP_UTF8, 0, stAppInfo.strTitle, stAppInfo.strTitle.GetLength(), NULL, 0 );
		wchar_t* pWideChar = (wchar_t*)calloc( n + 1, sizeof(wchar_t) );
		MultiByteToWideChar( CP_UTF8, 0, stAppInfo.strTitle, stAppInfo.strTitle.GetLength(), pWideChar, n );
		stAppInfo.strTitle = CString( pWideChar );


		if ( !stAppInfo.strLink.IsEmpty() ){
			m_vstAppInfo.push_back( stAppInfo );
			TRACE( "%s:%s\n", strKey[0], stAppInfo.strLink );
		}
	}

//	const int s  = APP_STN_W_SPACING;
	const int w  = m_sizeIcon.cx;
	const int h  = m_sizeIcon.cy;
	const int wd = APP_STN_W_DISTANCE;
	const int hd = APP_STN_H_DISTANCE;
	int l,t,r,b;
	UINT nIcoId;
	int nIdx;
	

//	HICON IconLarge;
	for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
		UINT nRet;
		nIdx = distance( m_vstAppInfo.begin(), i );
		if ( ! i->strLink.IsEmpty() ){
			//GetEnvironmentVariable();
			
			if ( i->strIcon.IsEmpty() ){
				if ( "\\" == i->strLink.Right( 1 ) ){
					nRet = ExtractIconEx( "Shell32.dll", 4, NULL, &(i->hIcon), 1 );		// opened folder icon
				} else {
					UINT IconNum = ExtractIconEx( i->strLink, -1, NULL, NULL, 0 );
					nRet = ExtractIconEx( i->strLink, 0, NULL, &(i->hIcon), 1 );
				}
			} else {
				UINT IconNum = ExtractIconEx( i->strIcon, -1, NULL, NULL, 0 );
				nIcoId = ( i->nIcoId < IconNum ) ? i->nIcoId : 0;
				nRet = ExtractIconEx( i->strIcon, nIcoId, NULL, &(i->hIcon), 1 );
			}
			if ( -1 == (INT)nRet ){
				nRet = ExtractIconEx( "imageres.dll", 11, NULL, &(i->hIcon), 1 );		// unknown executable file's icon.
				if ( nRet == -1 ){
					ASSERT(0);
				}
			}

			l = APP_STN_W_SPACING;
			t = APP_STN_TOP + nIdx * hd;
			r = APP_STN_W_DISTANCE;
			b = t + m_sizeIcon.cy;
			if ( NULL == i->hIcon ){
				ASSERT(0);
			} else {
				i->pStnIcon = new CTransparentImage;
				i->pStnIcon->Create( "", WS_CHILD | WS_VISIBLE | SS_ICON | SS_NOTIFY, CRect( l, t, r, b ), this, IDC_STN_HEAD + nIdx );
			//	i->pStnIcon->SetIcon( i->hIcon, 16, 16 );
				i->pStnIcon->SetIcon( i->hIcon, m_sizeIcon.cx, m_sizeIcon.cy );
				if ( ! i->strTip.IsEmpty() ){
					i->pTipCtl = new CToolTipCtrl;
					i->pTipCtl->Create( this );
					i->pTipCtl->AddTool( GetDlgItem( IDC_STN_HEAD + nIdx ), i->strTip );
				}
			}
		}

		if ( ! i->strTitle.IsEmpty() ){
			i->pStnTitle = new CStatic;
			i->rectTitle.SetRect( r + APP_STN_TITLE_SPACING, t + 1, r + 100, b );
			i->pStnTitle->Create( ""/*i->strTitle*/, WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOTIFY, i->rectTitle, this, IDC_STN_TITLE_HEAD + nIdx );

			i->pStnFont = new CFont;
			i->pStnFont->CreateFont( m_nFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS,
				CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH&FF_SWISS, /*"SimSun"*/m_strFontName );

			i->pStnTitle->SetFont( i->pStnFont );

		//	CRect rect;
			CSize size( 0, 0 );
		//	i->pStnTitle->GetWindowRect( rect );
		//	ScreenToClient( &rect );
			CClientDC dc( i->pStnTitle );
			dc.SelectObject( i->pStnFont );
		//	CFont *pOldFont = dc.SelectObject( this->GetFont() );	// this step is necessary
			//CString str;
			//i->pStnTitle->GetWindowText( str );
			//if( ::GetTextExtentPoint32( (HDC)dc, str, str.GetLength(), &size ) ){
			if( ::GetTextExtentPoint32( (HDC)dc, i->strTitle, i->strTitle.GetLength(), &size ) ){
			//	rect.right = rect.left + size.cx + 15; // size.cx has a little small sometimes.
			//	rect.bottom = rect.top + size.cy;
			//	m_nAppWidthTitle = ( m_nAppWidthTitle < rect.Width() ) ? rect.Width() : m_nAppWidthTitle;
				i->rectTitle.right = i->rectTitle.left + size.cx;
				i->rectTitle.bottom = i->rectTitle.top + size.cy;
				m_nAppWidthTitle = ( m_nAppWidthTitle < i->rectTitle.Width() ) ? i->rectTitle.Width() : m_nAppWidthTitle;
			} else {
				TRACE( "GetTextExtentPoint32 fail to get the size of text!\n" );
			//	i->pStnTitle->SetWindowText( "GetTextExtentPoint32 fail to get the size of text!" );
			}
			//i->pStnTitle->MoveWindow( rect );
			i->pStnTitle->MoveWindow( i->rectTitle );
		//	dc.SelectObject( pOldFont );

			i->rectTitle.top -= 3;
			i->rectTitle.bottom += 3;
			i->rectTitle.right += 10;
			//CFont* pFont = i->pStnTitle->GetFont();
			//LOGFONT LogFont;
			//pFont->GetLogFont(&LogFont);
			//LogFont.lfUnderline = 1;
			//LogFont.lfHeight = 12;
			//pFont->Detach();
			//pFont->CreateFontIndirect(&LogFont);
			//i->pStnTitle->SetFont( pFont );
			//pFont->Detach();
		}
	}

	::SetClassLongPtr( m_vstAppInfo.at(0).pStnIcon->m_hWnd, GCL_HCURSOR, (LONG)LoadCursor( NULL, IDC_HAND ) );

	m_nAppWidthTitle += APP_STN_W_SPACING + m_sizeIcon.cx + APP_STN_TITLE_SPACING + APP_STN_W_SPACING;

	m_sizeApp.cy = APP_STN_TOP + APP_STN_H_DISTANCE * m_vstAppInfo.size() + APP_STN_BOTTOM;

	UpdateUI( m_bIsShowTitle );

	SetWindowPos( &wndTopMost, nScreenX, nScreenY, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE );
}

void CMyDockDlg::SaveSetting( void )
{
	char szBuff[16];

	_itoa_s( m_rect.left, szBuff, 10 );
	WritePrivateProfileString( "options", "screen x", szBuff, m_strSettingFile );

	_itoa_s( m_rect.top, szBuff, 10 );
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
				CBrush brush( RGB( 220, 220, 220 ) );
				GetDC()->FillRect( &rect, &brush );
			} else {
				CBrush brush( RGB( 0, 0, 0 ) );
				GetDC()->FillRect( &rect, &brush );
			}
		}

		CWindowDC dc(this);
		USES_CONVERSION;
		for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
			DrawGlowingText( dc.m_hDC, A2W( i->strTitle ), i->rectTitle );
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
				this->SetWindowPos( NULL, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height() * seq / SEQ_NUM, SWP_SHOWWINDOW );
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
				this->SetWindowPos( NULL, 0, 0, m_rect.Width() * seq / SEQ_NUM, m_rect.Height(), SWP_NOMOVE );
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
				this->SetWindowPos( NULL, m_rect.left + m_rect.Width() * ( SEQ_NUM - seq ) / SEQ_NUM, m_rect.top, m_rect.Width(), m_rect.Height(), SWP_SHOWWINDOW );
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
		m_rect = rect;
		if ( m_rect.top <=0 ){
			m_enHidePosi = TOP;
			ModifyStyle( WS_THICKFRAME, NULL );
			this->SetWindowPos( NULL, m_rect.left, 0, m_rect.Width(), 2, SWP_NOCOPYBITS );

		//	m_rect.bottom -= m_rect.top;
			m_rect.bottom = m_sizeApp.cy;
			m_rect.top    =  0;
			m_bIsHiding = true;

			if ( m_osvi.dwMajorVersion >= 6 ){
				DwmExtendFrameIntoClientArea( m_hWnd, &m_marginsNone );
			}
		} else if ( m_rect.left <=0 ){
			m_enHidePosi = LEFT;
			ModifyStyle( WS_THICKFRAME, NULL );
			this->SetWindowPos( NULL, 0, m_rect.top, 2, m_rect.Height(), SWP_NOCOPYBITS );

		//	m_rect.right -= m_rect.left;
			m_rect.right = m_sizeApp.cx;
			m_rect.left  = 0;
			m_bIsHiding = true;

			if ( m_osvi.dwMajorVersion >= 6 ){
				DwmExtendFrameIntoClientArea( m_hWnd, &m_marginsNone );
			}
		} else if ( m_rect.right >= m_screenX ){ 
			m_enHidePosi = RIGHT;
			ModifyStyle( WS_THICKFRAME, NULL );
			this->SetWindowPos( NULL, m_screenX-2, m_rect.top, 2, m_rect.Height(), SWP_NOCOPYBITS );

			m_rect.left  = m_screenX - m_rect.Width();
			m_rect.right = m_screenX;//m_rect.left; //   m_rect.right - m_screenX
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
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE ){
		//m_obj.SetFocus();
		return TRUE;
	}

	//if ( dbgFlg && pMsg->message == WM_PAINT ){
	//	return TRUE;
	//}
//	( pMsg->message == 0x0113 ) ? 0 : TRACE( ",%04X", pMsg->message );

	for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
		if ( i->pTipCtl ){
			i->pTipCtl->RelayEvent(pMsg);
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
	if(!dMenu.LoadMenu(IDR_MENU1))
		AfxThrowResourceException();
	CMenu* pPopMenu=dMenu.GetSubMenu(0);
	ASSERT(pPopMenu!=NULL);
	pPopMenu->CheckMenuItem( ID_RCLICKMENU_SHOWTITLES, ( m_bIsShowTitle ? MF_CHECKED : MF_UNCHECKED ) );
	::GetCursorPos(&pt);
	pPopMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this );

	CDialogEx::OnRButtonUp(nFlags, point);
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

	UpdateUI( m_bIsShowTitle );
}

void CMyDockDlg::UpdateUI( bool bIsShowTitle )
{
	if ( bIsShowTitle ){
		for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
			if ( i->pStnTitle ){
				i->pStnTitle->ModifyStyle( 0, WS_VISIBLE );
			}
		}

		m_sizeApp.cx = m_nAppWidthTitle;
	} else {
		for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
			if ( i->pStnTitle ){
				i->pStnTitle->ModifyStyle( WS_VISIBLE, 0 );
			}
		}

		m_sizeApp.cx = APP_STN_W_SPACING + m_sizeIcon.cx + APP_STN_W_SPACING;
	}

	m_rect.right  = m_rect.left + m_sizeApp.cx;
	m_rect.bottom = m_rect.top  + m_sizeApp.cy;

	ModifyStyle( NULL, WS_THICKFRAME );
	SetWindowPos( &wndTopMost, 0, 0, m_rect.Width(), m_rect.Height(), SWP_SHOWWINDOW | SWP_NOMOVE );
	m_bIsHiding  = false;
	m_enHidePosi = NO;

	Invalidate( FALSE );	// do not erase the background
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