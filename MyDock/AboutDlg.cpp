// AboutDlg.cpp
//

#include "stdafx.h"
#include "MyDock.h"
#include "AboutDlg.h"
#include "afxdialogex.h"



IMPLEMENT_DYNAMIC(CAboutDlg, CDialogEx)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAboutDlg::IDD, pParent)
{

}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STN_ICON, m_stnIcon);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()



BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_stnIcon.ModifyStyle( NULL, SS_ICON | SS_CENTER );
	HICON hIcon = LoadIcon( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDR_MAINFRAME ) );
	m_stnIcon.SetIcon( hIcon );


	return TRUE;  // return TRUE unless you set the focus to a control

}
