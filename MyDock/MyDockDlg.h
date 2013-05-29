
// MyDockDlg.h : 
//

#pragma once
#include "TransparentImage.h"

#define TIMER_EVENT_ID_100MS		100

#define MAX_APP_NUM					128
#define	IDC_STN_HEAD				IDC_STN_APP1
#define IDC_STN_END					(IDC_STN_APP1 + MAX_APP_NUM)

#define MAX_APP_NUM					128
#define	IDC_STN_TITLE_HEAD			IDC_STN_TITLE1
#define IDC_STN_TITLE_END			(IDC_STN_TITLE1 + MAX_APP_NUM)


#define APP_STN_WIDTH				16
#define APP_STN_HEIGHT				16

#define APP_STN_TOP					10
#define APP_STN_BOTTOM				10

#define APP_STN_TITLE_SPACING		0

#define APP_STN_W_SPACING	 		5
#define APP_STN_H_SPACING	 		5
#define APP_STN_W_DISTANCE 			(APP_STN_W_SPACING + APP_STN_WIDTH)
#define APP_STN_H_DISTANCE 			(APP_STN_H_SPACING + APP_STN_HEIGHT)

#define APP_WIDTH					(APP_STN_W_SPACING + APP_STN_WIDTH + 500 + APP_STN_W_SPACING)
#define APP_HEIGHT					(APP_STN_TOP + APP_STN_H_DISTANCE * 10 + APP_STN_BOTTOM)


#define HOLD_TIME_BEFORE_HIDE		300		// ms
#define HOLD_TIME_BEFORE_SHOW		200	// ms
#define HOLD_TIME_BEFORE_SHOW_ID	1

#define SEQ_NUM						10
#define EVERY_TIME					( 20 / SEQ_NUM )

typedef enum _hide_posi{
	NO,
	LEFT,
	RIGHT,
	TOP
} enHidePosi;

typedef struct _app_info {
//	CStatic* pStnIcon;
	CTransparentImage* pStnIcon;
	CStatic* pStnTitle;
	CRect rectTitle;
	CFont* pStnFont;

	CString strTitle;
	CString strLink;
	CString strPara;
	CString strDir;
	
	CString strIcon;
	HICON hIcon;
	UINT nIcoId;
	
	CString strTip;
	CToolTipCtrl* pTipCtl;
} ST_APP_INFO;

// CMyDockDlg 
class CMyDockDlg : public CDialogEx
{
// 
public:
	CMyDockDlg(CWnd* pParent = NULL);	// 
	~CMyDockDlg();

	void LoadSetting( void );
	void SaveSetting( void );
	BOOL IsMouseInWindow( void );
	void DockedShow( void );
	void DockedHidden( bool bIsForceHide = false );

	void UpdateUI( bool bIsShowTitle );

	void DrawGlowingText( HDC hDC, LPWSTR szText, RECT &rcArea, DWORD dwTextFlags = DT_LEFT | DT_VCENTER | DT_SINGLELINE, int iGlowSize = 5 );

	enum { IDD = IDD_MYDOCK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	



protected:
	HICON m_hIcon;

	OSVERSIONINFO m_osvi;

	BOOL m_bIsAeroGlassEn;

	CBrush m_brush;
	CString m_strSettingFile;
	
	int m_screenX;
	int m_screenY;
	enHidePosi m_enHidePosi;
	DWORD m_dwLastActiveTime;
	DWORD m_dwTriggerTime;
	bool m_bIsDelaying;
	CRect m_rect;
	bool m_bIsHiding;
	bool m_bIsShowTitle;
	int m_nTitleMaxWidth;

	std::vector<ST_APP_INFO> m_vstAppInfo;

	bool m_bIsDraging;
	CPoint m_cpLBDown;

	HCURSOR m_hCursor;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void OnBnClickedBnApp(UINT nCtlId);
	void OnBnClickedBnAppTitle(UINT nCtlId);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRclickmenuSetting();
	afx_msg void OnRclickmenuClose();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRclickmenuShowtitles();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRclickmenuAbout();
};

