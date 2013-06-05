// LnkParser.cpp

#include "stdafx.h"
#include "LnkParser.h"



CLnkParser::CLnkParser( void )
	: m_pShellLink( NULL ), m_pPersistFile( NULL )
{
	HRESULT hr = S_OK;
lb_init:
	hr = Initialize();
	if ( hr == CO_E_NOTINITIALIZED )
	{
		hr = CoInitialize( NULL );
		if ( SUCCEEDED( hr ) )
		{
			goto lb_init;
		}
	}
	if ( FAILED( hr ) )
	{

		Uninitialize();

		throw ( _T("Initialize failed.") );
	}
}


CLnkParser::~CLnkParser( void )
{

	Uninitialize();
}


HRESULT CLnkParser::LoadFile( LPCWSTR lpFileName )
{
	HRESULT hr = S_OK;

	hr = m_pPersistFile->Load( lpFileName, STGM_READ );

	if ( FAILED(hr) )
	{
		return hr;
	}

	hr = m_pShellLink->Resolve( NULL, SLR_ANY_MATCH );

	if ( FAILED(hr) )
	{
		return hr;
	}
	return S_OK;
}


HRESULT CLnkParser::Initialize( void )
{
	HRESULT hr = S_OK;

	hr = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (LPVOID*)&m_pShellLink );

	if ( FAILED(hr) )
	{
		return hr;
	}


	hr = m_pShellLink->QueryInterface( IID_IPersistFile, (LPVOID*)&m_pPersistFile );

	if ( FAILED(hr) )
	{
		return hr;
	}
	return S_OK;
}


void CLnkParser::Uninitialize( void )
{

	if ( m_pPersistFile )
	{
		m_pPersistFile->Release();
		m_pPersistFile = NULL;
	}

	if ( m_pShellLink )
	{
		m_pShellLink->Release();
		m_pShellLink = NULL;
	}
}


HRESULT CLnkParser::GetTargetPath( LPTSTR lpStr, int cch )
{
	HRESULT hr = S_OK;
	hr = m_pShellLink->GetPath( lpStr, cch, NULL, SLGP_RAWPATH );
	if ( FAILED(hr) )
	{
		return hr;
	}
	return S_OK;
}


HRESULT CLnkParser::GetTargetIDList( LPITEMIDLIST *ppidl )
{
	HRESULT hr = S_OK;
	hr = m_pShellLink->GetIDList( ppidl );
	if ( FAILED(hr) )
	{
		return hr;
	}
	return S_OK;
}


HRESULT CLnkParser::GetTargetDescription( LPTSTR pszName, int cch )
{
	HRESULT hr = S_OK;
	hr = m_pShellLink->GetDescription( pszName, cch );
	if ( FAILED(hr) )
	{
		return hr;
	}
	return S_OK;
}


HRESULT CLnkParser::GetTargetWorkingDirectory( LPTSTR pszDir, int cch )
{
	HRESULT hr = S_OK;
	hr = m_pShellLink->GetWorkingDirectory( pszDir, cch );
	if ( FAILED(hr) )
	{
		return hr;
	}
	return S_OK;
}


HRESULT CLnkParser::GetTargetArguments( LPTSTR pszArgs, int cch )
{
	HRESULT hr = S_OK;
	hr = m_pShellLink->GetArguments( pszArgs, cch );
	if ( FAILED(hr) )
	{
		return hr;
	}
	return S_OK;
}


HRESULT CLnkParser::GetTargetHotkey( WORD *pwHotkey )
{
	HRESULT hr = S_OK;
	hr = m_pShellLink->GetHotkey( pwHotkey );
	if ( FAILED(hr) )
	{
		return hr;
	}
	return S_OK;
}


HRESULT CLnkParser::GetTargetShowCmd( int *piShowCmd )
{
	HRESULT hr = S_OK;
	hr = m_pShellLink->GetShowCmd( piShowCmd );
	if ( FAILED(hr) )
	{
		return hr;
	}
	return S_OK;
}


HRESULT CLnkParser::GetTargetIconLocation( LPTSTR pszIconPath, int cch, int *piIcon )
{
	HRESULT hr = S_OK;
	hr = m_pShellLink->GetIconLocation( pszIconPath, cch, piIcon );
	if ( FAILED(hr) )
	{
		return hr;
	}
	return S_OK;
}


