
// MyDockDlg.h : ͷ�ļ�
//

#pragma once

typedef enum _hide_posi{
	NO,  //�ǿ���
	LEFT, //����
	RIGHT,//����
	TOP	  //����
} enHidePosi;

// CMyDockDlg �Ի���
class CMyDockDlg : public CDialogEx
{
// ����
public:
	CMyDockDlg(CWnd* pParent = NULL);	// ��׼���캯��

	BOOL IsMouseInWindow( void );
	void DockedShow( void );
	void DockedHidden( void );
// �Ի�������
	enum { IDD = IDD_MYDOCK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	int m_screenX;
	int m_screenY;
	enHidePosi m_enHidePosi;
	DWORD m_dwLastActiveTime;
	CRect m_rect;
	// ���ɵ���Ϣӳ�亯��
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
