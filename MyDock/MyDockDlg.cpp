
// MyDockDlg.cpp : 
//

#include "stdafx.h"
#include "MyDock.h"
#include "MyDockDlg.h"
#include "afxdialogex.h"

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
	ON_CONTROL_RANGE(STN_CLICKED, IDC_STN_HEAD, IDC_STN_END, &CMyDockDlg::OnBnClickedBnApp)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
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

	SetWindowPos( &wndTopMost, NULL, NULL, APP_WIDTH, APP_HEIGHT, SWP_NOMOVE );

	m_screenX = GetSystemMetrics( SM_CXSCREEN );
	m_screenY = GetSystemMetrics( SM_CYSCREEN );
	m_dwLastActiveTime = GetTickCount();
	m_enHidePosi = NO;
	SetTimer( TIMER_EVENT_ID_100MS, 100, NULL );

	m_bIsDraging = false;

	OleInitialize( NULL );

	Loading();

	return TRUE;  
}

CMyDockDlg::~CMyDockDlg(){
	
	for (std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
		if ( i->pStn ){
			delete i->pStn;
		}

		if ( i->hIcon ){
			DestroyIcon( i->hIcon );
		}

		if ( i->pTipCtl ){
			delete i->pTipCtl;
		}
	}
}



void CMyDockDlg::Loading( void ){
	CString strKey[6] = { "link", "para", "workdir", "tip", "icon", "icoid" };
	CString strSettingFile;
	CString strBuff;
	CString strSection;
//	UINT nSettingItemCnt = 0;
	UINT i;

	GetModuleFileName( NULL, strSettingFile.GetBuffer(MAX_PATH) , MAX_PATH );
	strSettingFile.ReleaseBuffer();
	strSettingFile = strSettingFile.Left( strSettingFile.ReverseFind('\\') ) + "\\setting.ini";
	TRACE( "%s\n", strSettingFile );

	for ( i = 0; i < MAX_APP_NUM; i++ ){
		ST_APP_INFO stAppInfo;
		stAppInfo.pStn = NULL;
		stAppInfo.hIcon = NULL;
		stAppInfo.nIcoId = 0;
		stAppInfo.pTipCtl = NULL;

		strSection.Format( "App%d", i + 1  );
		GetPrivateProfileString( strSection, strKey[0], "", stAppInfo.strLink.GetBuffer(MAX_PATH), MAX_PATH, strSettingFile );
		stAppInfo.strLink.ReleaseBuffer();

		GetPrivateProfileString( strSection, strKey[1], "", stAppInfo.strPara.GetBuffer(MAX_PATH), MAX_PATH, strSettingFile );
		stAppInfo.strPara.ReleaseBuffer();

		GetPrivateProfileString( strSection, strKey[2], "", stAppInfo.strDir.GetBuffer(MAX_PATH), MAX_PATH, strSettingFile );
		stAppInfo.strDir.ReleaseBuffer();

		GetPrivateProfileString( strSection, strKey[3], "", stAppInfo.strTip.GetBuffer(MAX_PATH), MAX_PATH, strSettingFile );
		stAppInfo.strTip.ReleaseBuffer();

		GetPrivateProfileString( strSection, strKey[4], "", stAppInfo.strIcon.GetBuffer(MAX_PATH), MAX_PATH, strSettingFile );
		stAppInfo.strIcon.ReleaseBuffer();

		GetPrivateProfileString( strSection, strKey[5], "", strBuff.GetBuffer(MAX_PATH), MAX_PATH, strSettingFile );
		strBuff.ReleaseBuffer();
		stAppInfo.nIcoId = strtol( strBuff, NULL, 10 );

		if ( !stAppInfo.strLink.IsEmpty() ){
			m_vstAppInfo.push_back( stAppInfo );
			TRACE( "%s:%s\n", strKey[0], stAppInfo.strLink );
		}
	}

	const int s  = APP_STN_SPACING;
	const int w  = APP_STN_WIDTH;
	const int h  = APP_STN_HEIGHT;
	const int wd = APP_STN_W_DISTANCE;
	const int hd = APP_STN_H_DISTANCE;
	int l,t,r,b;
	UINT nID;
	UINT nIcoId;
//	HICON IconLarge;
	for ( std::vector<ST_APP_INFO>::iterator i = m_vstAppInfo.begin(); i != m_vstAppInfo.end(); i++ ){
		if ( i->strLink.IsEmpty() ){
			
		}else{
			//GetEnvironmentVariable();
			i->pStn = new CStatic;
			if ( i->strIcon.IsEmpty() ){
				if ( "\\" == i->strLink.Right( 1 ) ){
					ExtractIconEx( "Shell32.dll", 4, NULL, &(i->hIcon), 1 );		// opened folder icon
				} else {
					UINT IconNum = ExtractIconEx( i->strLink, -1, NULL, NULL, 0 );
					ExtractIconEx( i->strLink, 0, NULL, &(i->hIcon), 1 );
				}
			} else {
				UINT IconNum = ExtractIconEx( i->strIcon, -1, NULL, NULL, 0 );
				nIcoId = ( i->nIcoId < IconNum ) ? i->nIcoId : 0;
				ExtractIconEx( i->strIcon, nIcoId, NULL, &(i->hIcon), 1 );
			}

			l = s;
			t = s + distance( m_vstAppInfo.begin(), i ) * hd;
			r = wd;
			b = t + h;
			nID = IDC_STN_HEAD + distance( m_vstAppInfo.begin(), i );
			if ( NULL == i->hIcon ){
				
			} else {
				i->pStn->Create( "", WS_CHILD | WS_VISIBLE | SS_ICON | SS_NOTIFY, CRect( l, t, r, b ), this, nID );
				i->pStn->SetIcon( i->hIcon );
				if ( ! i->strTip.IsEmpty() ){
					i->pTipCtl = new CToolTipCtrl;
					i->pTipCtl->Create( this );
					i->pTipCtl->AddTool( GetDlgItem( nID ),i->strTip );
				}
			}
		}
	}
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
	if ( m_bIsDraging ){
		CRect rc;
		GetWindowRect( &rc );
		long x = (long)rc.left + (long)point.x - (long)m_cpLBDown.x;
		long y = (long)rc.top + (long)point.y - (long)m_cpLBDown.y;

		SetWindowPos( this, x, y, rc.Width(), rc.Height(), SWP_NOZORDER );
	}
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

	//	ModifyStyle( NULL, WS_SYSMENU | WS_THICKFRAME );

		int seq = 0;
		switch( m_enHidePosi ){
		case TOP:
			while ( ++seq <= SEQ_NUM ){
				this->SetWindowPos( NULL, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height() * seq / SEQ_NUM, SWP_NOCOPYBITS );
			    Sleep( EVERY_TIME );
			}
			break;
		case LEFT:
			while ( ++seq <= SEQ_NUM ){
				this->SetWindowPos( NULL, m_rect.left, m_rect.top, m_rect.Width() * seq / SEQ_NUM, m_rect.Height(), SWP_NOCOPYBITS );
				Sleep( EVERY_TIME );		
			}
			break;
		case RIGHT:
			while ( ++seq <= SEQ_NUM ){
				this->SetWindowPos( NULL, m_rect.left + m_rect.Width() * ( SEQ_NUM - seq ) / SEQ_NUM, m_rect.top, m_rect.Width(), m_rect.Height(), SWP_NOCOPYBITS );
				Sleep( EVERY_TIME );
			}
			break;
		}
		m_enHidePosi = NO;
	}
}

void CMyDockDlg::DockedHidden( bool bIsForceHide ){
	CRect rect;
	GetWindowRect( &rect );
	
	if ( ( m_enHidePosi ==NO && !IsMouseInWindow() ) || 
		( bIsForceHide ) ){
		m_rect = rect;
		if ( m_rect.top <=0 ){
			m_enHidePosi = TOP;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, m_rect.left, 0, m_rect.Width(), 2, SWP_NOCOPYBITS );

			m_rect.bottom -= m_rect.top;
			m_rect.top    =  0;
			
		} else if ( m_rect.left <=0 ){ 
			m_enHidePosi = LEFT;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, 0, m_rect.top, 2, m_rect.Height(), SWP_NOCOPYBITS );

			m_rect.right -= m_rect.left;
			m_rect.left  = 0;
			
		} else if ( m_rect.right >= m_screenX ){ 
			m_enHidePosi = RIGHT;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, m_screenX-2, m_rect.top, 2, m_rect.Height(), SWP_NOCOPYBITS );

			m_rect.left  = m_screenX - m_rect.Width();
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
	//if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE ){
	//	//m_obj.SetFocus();
	//	return TRUE;
	//}

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
	ShellExecute( NULL, "open", info.strLink, info.strPara, info.strDir, SW_SHOW );
	DockedHidden( true );
}


void CMyDockDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bIsDraging = true;
	m_cpLBDown = point;
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMyDockDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bIsDraging = false;
	CDialogEx::OnLButtonUp(nFlags, point);
}
