//////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2003-2007, Arne Bockholdt, github@bockholdt.info
//
// This file is part of Direct Sound Control.
//
// Direct Sound Control is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Direct Sound Control is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Direct Sound Control.  If not, see <http://www.gnu.org/licenses/>.
//
//////////////////////////////////////////////////////////////////////////////////////////////
//
// Updated by Elisha Riedlinger 2017
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <atlstr.h>
#include "mmsystem.h"
#include "dsound.h"
#include "DSoundTypes.h"
#include "idirectsoundex.h"
#include "idirectsoundclassfactoryex.h"

//////////////////////////////////////////////////////////////////////////////////////////////

IDirectSoundClassFactoryEx::IDirectSoundClassFactoryEx(void)
{
#ifdef ENABLE_LOG
	m_cszClassName = IDIRECTSOUNDCLASSFACTORYEX_CLASS_NAME;

	if( g_bLogSystem == true )
		LogMessage( m_cszClassName, this, "Constructor called....");
#endif // ENABLE_LOG

}

//////////////////////////////////////////////////////////////////////////////////////////////

IDirectSoundClassFactoryEx::~IDirectSoundClassFactoryEx(void)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( m_cszClassName, this, "Destructor called....");
#endif // ENABLE_LOG
}

//////////////////////////////////////////////////////////////////////////////////////////////


HRESULT IDirectSoundClassFactoryEx::QueryInterface( REFIID refIID, LPVOID * pVoid)
{
*pVoid = NULL;

	if( refIID == IID_IClassFactory )
	{
		LPVOID pTemp;
		HRESULT hRes =  m_lpClassFactory->QueryInterface( refIID, &pTemp );

		if( hRes != S_OK )
		{
#ifdef ENABLE_LOG
			if( g_bLogSystem == true )
				::LogMessage( m_cszClassName, this, "QueryInterface for interface IID_IClassFactory failed...");
#endif // ENABLE_LOG
		
			return hRes;	
		}

		m_lpClassFactory = (LPCLASSFACTORY) pTemp;

		*pVoid = (LPVOID) this;

#ifdef ENABLE_LOG
		if( g_bLogSystem == true )
			::LogMessage( m_cszClassName, this, "QueryInterface called,Interface=IID_IClassFactory");
#endif // ENABLE_LOG

		return S_OK;
	}

#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( m_cszClassName, this, "QueryInterface called for unknown IID interface ....");
#endif // ENABLE_LOG

	return m_lpClassFactory->QueryInterface( refIID, pVoid );
}

//////////////////////////////////////////////////////////////////////////////////////////////

ULONG IDirectSoundClassFactoryEx::AddRef()
{
	ULONG nRefCnt = m_lpClassFactory->AddRef();

#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
	{
		CString sLogString;
		sLogString.Format("AddRef called....,nRefCnt=%u", nRefCnt ); 

		::LogMessage( m_cszClassName, this, sLogString.GetBuffer());
	}
#endif // ENABLE_LOG

	return nRefCnt;
}

//////////////////////////////////////////////////////////////////////////////////////////////

ULONG IDirectSoundClassFactoryEx::Release()
{
	ULONG nRefCnt = m_lpClassFactory->Release();

#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
	{
		CString sLogString;
		sLogString.Format("Release called....,nRefCnt=%u", nRefCnt ); 

		::LogMessage( m_cszClassName, this, sLogString.GetBuffer());
	}
#endif // ENABLE_LOG
	
	if ( nRefCnt == 0 )
	{
		delete this;
		return 0;
	}

	return nRefCnt;
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IDirectSoundClassFactoryEx::CreateInstance( IUnknown *pUnkOuter, REFIID riid, void **ppvObject)
{
	*ppvObject = NULL;

	if(( riid == IID_IDirectSound ) || ( riid == IID_IDirectSound8 ))
	{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		::LogMessage( m_cszClassName, this, "CreateInstance called for IID_IDirectSound(8) interface....");
#endif // ENABLE_LOG

		IDirectSound8Ex* pDSX = new IDirectSound8Ex;

		HRESULT hRes = m_lpClassFactory->CreateInstance( pUnkOuter, riid, (LPVOID*)&( pDSX->m_lpDirectSound8 ) );

		if( hRes != S_OK )
		{
#ifdef ENABLE_LOG
			if( g_bLogSystem == true )
				::LogMessage( m_cszClassName, this, "DirectSoundCreate failed....");
#endif // ENABLE_LOG

			delete pDSX;
			return S_FALSE;
		}

		*ppvObject = (LPDIRECTSOUND) pDSX;

		return S_OK;
	}

#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		::LogMessage( m_cszClassName, this, "CreateInstance called for unknown IID interface....");
#endif // ENABLE_LOG


	return m_lpClassFactory->CreateInstance(pUnkOuter, riid, ppvObject);
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IDirectSoundClassFactoryEx::LockServer( BOOL fLock )
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		::LogMessage( m_cszClassName, this, "LockServer called....");
#endif // ENABLE_LOG

	return m_lpClassFactory->LockServer(fLock);
}

//////////////////////////////////////////////////////////////////////////////////////////////
