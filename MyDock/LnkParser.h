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
	HRESULT CLnkParser::GetTargetIDList( LPITEMIDLIST *ppidl );
	HRESULT CLnkParser::GetTargetDescription( LPTSTR pszName, int cch );
	HRESULT CLnkParser::GetTargetWorkingDirectory( LPTSTR pszDir, int cch );
	HRESULT CLnkParser::GetTargetArguments( LPTSTR pszArgs, int cch );
	HRESULT CLnkParser::GetTargetHotkey( WORD *pwHotkey );
	HRESULT CLnkParser::GetTargetShowCmd( int *piShowCmd );
	HRESULT CLnkParser::GetTargetIconLocation( LPTSTR pszIconPath, int cch, int *piIcon );

private:
	HRESULT Initialize( void );
	void Uninitialize( void );
private:
	IShellLink *m_pShellLink;
	IPersistFile *m_pPersistFile;
};
