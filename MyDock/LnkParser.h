// lnkparser.h


#pragma once

EXTERN_C const CLSID CLSID_ShellLink;

#ifdef UNICODE
#define IID_IShellLink  IID_IShellLinkW
#else
#define IID_IShellLink  IID_IShellLinkA
#endif // !UNICODE


class CLnkParser
{
public:
	CLnkParser( void );
	~CLnkParser( void );
	
	HRESULT LoadFile( LPCWSTR lpFileName );
	HRESULT GetTargetPath( LPTSTR lpStr, int cch );
	HRESULT GetTargetIDList( LPITEMIDLIST *ppidl );
	HRESULT GetTargetDescription( LPTSTR pszName, int cch );
	HRESULT GetTargetWorkingDirectory( LPTSTR pszDir, int cch );
	HRESULT GetTargetArguments( LPTSTR pszArgs, int cch );
	HRESULT GetTargetHotkey( WORD *pwHotkey );
	HRESULT GetTargetShowCmd( int *piShowCmd );
	HRESULT GetTargetIconLocation( LPTSTR pszIconPath, int cch, int *piIcon );

private:
	HRESULT Initialize( void );
	void Uninitialize( void );
private:
	IShellLink *m_pShellLink;
	IPersistFile *m_pPersistFile;
};
