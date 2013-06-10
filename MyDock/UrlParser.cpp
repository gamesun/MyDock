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
	DWORD dwLen = GetPrivateProfileString( "InternetShortcut", "URL", "", lpStr, cch, m_strFileName );

	return ( 0 < dwLen ) ? S_OK : -1;
}

HRESULT CUrlParser::GetTargetIconLocation( LPTSTR pszIconPath, int cch, int *piIcon )
{
	GetPrivateProfileString( "InternetShortcut", "IconFile", "", pszIconPath, cch, m_strFileName );
	//DWORD dwErrNo = GetLastError();									//when m_strFileName is not found, dwErrNo is 2,
	//if ( NO_ERROR != dwErrNo ){										//but if m_strFileName is exist and "IconFile" is not exist,
	//	ShowError( dwErrNo, "CUrlParser::GetTargetIconLocation" );		//dwErrNo is still fucking 2 !!!!!
	//	return dwErrNo;
	//}
	char szBuff[16];
	GetPrivateProfileString( "InternetShortcut", "IconIndex", "0", szBuff, cch, m_strFileName );
	//dwErrNo = GetLastError();
	//if ( NO_ERROR != dwErrNo ){
	//	ShowError( dwErrNo, "CUrlParser::GetTargetIconLocation" );
	//	return dwErrNo;
	//}
	*piIcon = strtol( szBuff, NULL, 10 );

	return S_OK;
}
