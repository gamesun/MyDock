
// MyDockDlg.h : 头文件
//

#pragma once

typedef enum _hide_posi{
	NO,  //非靠边
	LEFT, //靠左
	RIGHT,//靠右
	TOP	  //靠上
} enHidePosi;

// CMyDockDlg 对话框
class CMyDockDlg : public CDialogEx
{
// 构造
public:
	CMyDockDlg(CWnd* pParent = NULL);	// 标准构造函数

	BOOL IsMouseInWindow( void );
	void DockedShow( void );
	void DockedHidden( void );
// 对话框数据
	enum { IDD = IDD_MYDOCK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	int m_screenX;
	int m_screenY;
	enHidePosi m_enHidePosi;
	DWORD m_dwLastActiveTime;
	CRect m_rect;
	// 生成的消息映射函数
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
};
