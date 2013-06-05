

#pragma once

class CUrlParser
{
public:
	CUrlParser( void ){};
	~CUrlParser( void ){};
	
	HRESULT LoadFile( LPCWSTR lpFileName );
	HRESULT GetTargetUrl( LPTSTR lpStr, int cch );
	HRESULT GetTargetIconLocation( LPTSTR pszIconPath, int cch, int *piIcon );

private:
	CString m_strFileName;
private:

};
