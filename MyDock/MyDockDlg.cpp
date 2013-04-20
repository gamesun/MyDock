
// MyDockDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyDock.h"
#include "MyDockDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyDockDlg �Ի���




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


// CMyDockDlg ��Ϣ�������

BOOL CMyDockDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_screenX = GetSystemMetrics( SM_CXSCREEN );
	m_screenY = GetSystemMetrics( SM_CYSCREEN );
	m_dwLastActiveTime = GetTickCount();
	m_enHidePosi = NO;
	SetTimer( 100, 100, NULL );


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyDockDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyDockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyDockDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_dwLastActiveTime = GetTickCount();

	CDialogEx::OnMouseMove(nFlags, point);
}


void CMyDockDlg::OnNcMouseMove(UINT nHitTest, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	
	if( m_enHidePosi != NO && IsMouseInWindow() ){//�����أ���ʾ��ֵ
		//�ָ���ʽ
		ModifyStyle( NULL, WS_SYSMENU | WS_THICKFRAME );
		//��ԭ��С
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
		if ( m_rect.top <=0 ){//����
			m_enHidePosi = TOP;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, m_rect.left, 0, m_rect.right - m_rect.left, 2, SWP_NOCOPYBITS );
			//����
			//���ƶ�
			m_rect.bottom -= m_rect.top;
			m_rect.top    =  0;
			
		} else if ( m_rect.left <=0 ){ //����
			m_enHidePosi = LEFT;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, 0, m_rect.top, 2, m_rect.bottom - m_rect.top, SWP_NOCOPYBITS );
			//���ƶ�
			m_rect.right -= m_rect.left;
			m_rect.left  = 0;
			
		} else if ( m_rect.right >= m_screenX ){ //����
			m_enHidePosi = RIGHT;
			ModifyStyle( WS_THICKFRAME | WS_SYSMENU, NULL );
			this->SetWindowPos( NULL, m_screenX-2, m_rect.top, 2, m_rect.bottom-m_rect.top, SWP_NOCOPYBITS );
			//���ƶ�
			m_rect.left  = m_screenX - ( m_rect.right - m_rect.left );
			m_rect.right = m_screenX;//m_rect.left; //ƫ����   m_rect.right - m_screenX
			
		} else {
			m_enHidePosi = NO;
		}	
	}
}



void CMyDockDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���

	//CDialogEx::OnOK();
}


void CMyDockDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���

	CDialogEx::OnCancel();
}

BOOL CMyDockDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE ){
		//m_obj.SetFocus();
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
