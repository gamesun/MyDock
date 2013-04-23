
// MyDockDlg.h : 
//

#pragma once


#define MAX_APP_NUM		128
#define	IDC_BN_HEAD		IDC_BN_APP1
#define IDC_BN_END		IDC_BN_APP1 + MAX_APP_NUM

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
	void DockedHidden( void );

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
	std::vector<CButton*> m_vpbnApp;


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
};
