// UrlParser.cpp


#include "stdafx.h"
#include "UrlParser.h"


HRESULT CUrlParser::LoadFile( LPCWSTR lpFileName )
{
	USES_CONVERSION;
	m_strFileName.Format( "%s", W2A(lpFileName) );

	return S_OK;
}

HRESULT CUrlParser::GetTargetUrl( LPTSTR lpStr, int cch )
{
	GetPrivateProfileString( "InternetShortcut", "URL", "", lpStr, cch, m_strFileName );

	return S_OK;
}

HRESULT CUrlParser::GetTargetIconLocation( LPTSTR pszIconPath, int cch, int *piIcon )
{
	GetPrivateProfileString( "InternetShortcut", "IconFile", "", pszIconPath, cch, m_strFileName );

	char szBuff[16];
	GetPrivateProfileString( "InternetShortcut", "IconIndex", "0", szBuff, cch, m_strFileName );
	*piIcon = strtol( szBuff, NULL, 10 );

	return S_OK;
}