
// MyDock.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMyDockApp:
// �йش����ʵ�֣������ MyDock.cpp
//

class CMyDockApp : public CWinApp
{
public:
	CMyDockApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMyDockApp theApp;