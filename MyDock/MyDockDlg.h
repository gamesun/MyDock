
// MyDockDlg.h : 
//

#pragma once

#define TIMER_EVENT_ID_100MS		100

#define MAX_APP_NUM					128
#define	IDC_STN_HEAD				IDC_STN_APP1
#define IDC_STN_END					(IDC_STN_APP1 + MAX_APP_NUM)


#define APP_STN_WIDTH				20
#define APP_STN_HEIGHT				20

#define APP_STN_SPACING	 			3
#define APP_STN_W_DISTANCE 			(APP_STN_SPACING + APP_STN_WIDTH)
#define APP_STN_H_DISTANCE 			(APP_STN_SPACING + APP_STN_HEIGHT)

#define APP_WIDTH					(APP_STN_SPACING + APP_STN_WIDTH + APP_STN_SPACING)
#define APP_HEIGHT					(APP_STN_SPACING + APP_STN_HEIGHT * 10 + APP_STN_SPACING)

typedef enum _hide_posi{
	NO,
	LEFT,
	RIGHT,
	TOP
} enHidePosi;

// CMyDockDlg 
class CMyDockDlg : public CDialogEx
{
// 
public:
	CMyDockDlg(CWnd* pParent = NULL);	// 
	~CMyDockDlg();

	void Loading( void );
	BOOL IsMouseInWindow( void );
	void DockedShow( void );
	void DockedHidden( bool bIsForceHide = false );

	enum { IDD = IDD_MYDOCK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	



protected:
	HICON m_hIcon;
	int m_screenX;
	int m_screenY;
	enHidePosi m_enHidePosi;
	DWORD m_dwLastActiveTime;
	CRect m_rect;
	
	std::vector<CString> m_vstrApp;
	//std::vector<CButton*> m_vpstnApp;
	std::vector<CStatic*> m_vpstnApp;
	
	std::vector<HICON> m_vhIcon;

	bool m_bIsDraging;
	CPoint m_cpLBDown;

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
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
