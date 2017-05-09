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
// DSoundCtrl.cpp : Defines the initialization routines for the DLL.
//
//////////////////////////////////////////////////////////////////////////////////////////////
//
// Updated by Elisha Riedlinger 2017
//
//////////////////////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN

#ifndef _DEBUG
//#define new DEBUG_NEW		//<----TODO: fix this line
#endif

//////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include <atlstr.h>
#include <atltime.h>
#include <assert.h>
#include <atltrace.h>
#include "mmsystem.h"
#include "dsound.h"
#include "DSoundTypes.h"
#include "DSoundCtrl.h"
#include "IDirectSoundClassFactoryEx.h"

//////////////////////////////////////////////////////////////////////////////////////////////

LPDSENUMCALLBACKA	g_pAppDSEnumCallbackA;
LPDSENUMCALLBACKW	g_pAppDSEnumCallbackW;

//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_LOG

//////////////////////////////////////////////////////////////////////////////////////////////

const char* g_cszClassName = "ExportFunction";

//////////////////////////////////////////////////////////////////////////////////////////////

void LogMessage(const char* szClassName, void* pInstance, char* szMessage )
{	
	char buffer[1024];

	if( szClassName != NULL )
		sprintf_s( buffer, "%0.8u,%s,%u,%s\n", ::timeGetTime() - g_dwStartTime, szClassName, (unsigned long) pInstance, szMessage );
	else
		sprintf_s( buffer, ",,,%s\n", szMessage );

	FILE* f; 
		
	::fopen_s( &f, LOG_FILE_NAME, "a+tc" );
	::fwrite( buffer, 1 , strlen( buffer), f );
	::fclose( f );

	ATLTRACE( buffer );
}

//////////////////////////////////////////////////////////////////////////////////////////////

CString GetDirectXVersionFromReg()
{
	HKEY			RegKey;
    unsigned long	iSize = 255;
	DWORD			dwType;
	CString			sTemp = "";

	if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\DirectX",0,KEY_ALL_ACCESS,&RegKey) == ERROR_SUCCESS)
	{
		RegQueryValueEx(RegKey, "Version",NULL,&dwType,(BYTE*)  sTemp.GetBuffer(255), &iSize);

		sTemp.ReleaseBuffer();
		RegCloseKey(RegKey);
	}

	return sTemp;
}

//////////////////////////////////////////////////////////////////////////////////////////////

CString GetOSVersion()		//<----TODO: update this function
{
	CString			sTemp = "";
	CString			sOSName = "Unknown platform";

	OSVERSIONINFO oOS_version;
	oOS_version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); 
	::GetVersionEx( &oOS_version );

	switch( oOS_version.dwPlatformId )
	{
		case VER_PLATFORM_WIN32_WINDOWS : 
			{
				switch(oOS_version.dwMinorVersion)
				{
					case 0   : sOSName = "Windows 95"; break;
					case 10  : sOSName = "Windows 98"; break;
					case 90  : sOSName = "Windows ME"; break;
					default  : sOSName = "Unknown Windows 9x-based version";
				}					
				break;
			}
			break;
		case VER_PLATFORM_WIN32_NT :
			{
				switch( oOS_version.dwMajorVersion )
				{
					case 3  : sOSName = "Windows NT 3.x (not supported)"; break;
					case 4  : sOSName = "Windows NT 4 (not supported)"; break;
					case 5  :
						switch( oOS_version.dwMinorVersion )
						{
							case 0 : sOSName	= "Windows 2000"; break;
							case 1 : sOSName	= "Windows XP"; break;
							case 2 : sOSName	= "Windows Server 2003 (R2) or Windows XP Professional x64"; break;
							default : sOSName	= "Unknown Windows 2k-based version";
						}
						break;
					case 6  : 
						switch( oOS_version.dwMinorVersion )
						{
							case 0 : sOSName	= "Windows Vista or Windows Server 2008 (not tested)"; break;
							default : sOSName	= "Unknown Windows Vista-based version";
						}
						break;
		
					default : sOSName = "Unknown Windows NT-based version";
				}				
				break;
			}
	}

	sTemp.Format(" V%d.%d Build %u %s",oOS_version.dwMajorVersion,oOS_version.dwMinorVersion,(oOS_version.dwBuildNumber&0xffff),oOS_version.szCSDVersion );
	return sOSName + sTemp;
}

//////////////////////////////////////////////////////////////////////////////////////////////


BOOL CALLBACK DSEnumCallback( LPGUID  lpGuid,    LPCSTR  lpcstrDescription,  LPCSTR  lpcstrModule,   LPVOID  lpContext  )
{
	return ((CDSoundCtrlApp*)lpContext)->EnumCallback( lpGuid, lpcstrDescription, lpcstrModule );
}

//////////////////////////////////////////////////////////////////////////////////////////////

#endif  // ENABLE_LOG

//////////////////////////////////////////////////////////////////////////////////////////////
// CDSoundCtrlApp
//
// The one and only CDSoundCtrlApp object

CDSoundCtrlApp theApp;

//////////////////////////////////////////////////////////////////////////////////////////////
// CDSoundCtrlApp construction

CDSoundCtrlApp::CDSoundCtrlApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
#ifdef ENABLE_LOG
	m_cszClassName = CDSOUNDCTRLAPP_CLASS_NAME;
#endif // ENABLE_LOG
}

//////////////////////////////////////////////////////////////////////////////////////////////

CDSoundCtrlApp::~CDSoundCtrlApp()
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( m_cszClassName, this, "Destructor called....");
#endif // ENABLE_LOG

	::FreeLibrary( g_hDLL );

#ifdef ENABLE_LOG
	if( g_bDebugBeep == true )
		Beep( 3000, 100 );

	if( g_bLogSystem == true )
	{
		LogMessage( m_cszClassName, this, "Original DSOUND.DLL unloaded....");

		CString sMessage;
		CTime oTime = CTime::GetCurrentTime();

		sMessage.Format( "********* DirectSound Control %s  Logging ended at %s *********", PROGRAM_VERSION, oTime.Format("%B %d, %Y %H:%M:%S") );
		::LogMessage( (const char*) NULL, NULL, sMessage.GetBuffer());
	}

#endif // ENABLE_LOG
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CDSoundCtrlApp initialization

BOOL CDSoundCtrlApp::InitInstance()
{
	//
	// Read configuration from file
	//

#ifdef ENABLE_LOG
	g_bLogSystem					= this->ReadConfig( SECTION_DEBUG, KEY_LOGSYSTEM, g_bLogSystem ) > 0;
	g_bLogDirectSound				= this->ReadConfig( SECTION_DEBUG, KEY_LOGDIRECTSOUND, g_bLogDirectSound ) > 0;
	g_bLogDirectSoundBuffer			= this->ReadConfig( SECTION_DEBUG, KEY_LOGDIRECTSOUNDBUFFER, g_bLogDirectSoundBuffer ) > 0;
	g_bLogDirectSound3DBuffer		= this->ReadConfig( SECTION_DEBUG, KEY_LOGDIRECTSOUND3DBUFFER, g_bLogDirectSound3DBuffer ) > 0;
	g_bLogDirectSound3DListener		= this->ReadConfig( SECTION_DEBUG, KEY_LOGDIRECTSOUND3DLISTENER, g_bLogDirectSound3DListener ) > 0;
	g_bDebugBeep					= this->ReadConfig( SECTION_DEBUG, KEY_DEBUGBEEP, g_bDebugBeep ) > 0;
#endif // ENABLE_LOG

	g_nNum2DBuffers					= this->ReadConfig( SECTION_TWEAK, KEY_NUM2DBUFF, 0 );
	g_nNum3DBuffers					= this->ReadConfig( SECTION_TWEAK, KEY_NUM3DBUFF, 0 );
	g_bForceCertification			= this->ReadConfig( SECTION_TWEAK, KEY_FORCECERT, FALSE ) > 0;
	g_bForceExclusiveMode			= this->ReadConfig( SECTION_TWEAK, KEY_FORCEEXCLUSIVEMODE, FALSE ) > 0;
	g_bForceSoftwareMixing			= this->ReadConfig( SECTION_TWEAK, KEY_FORCESOFTMIX, FALSE ) > 0;
	g_bForceHardwareMixing			= this->ReadConfig( SECTION_TWEAK, KEY_FORCEHARDMIX, FALSE ) > 0;
	g_bPreventSpeakerSetup			= this->ReadConfig( SECTION_TWEAK, KEY_PREVSPEAKSETUP, FALSE ) > 0;
	g_bForceHQSoftware3D			= this->ReadConfig( SECTION_TWEAK, KEY_HQSOFT3D, FALSE ) > 0;
	g_bForceNonStatic				= this->ReadConfig( SECTION_TWEAK, KEY_FORCENONSTATIC, FALSE ) > 0;
	g_bForceVoiceManagement			= this->ReadConfig( SECTION_TWEAK, KEY_FORCEVOICEMANAGEMENT, FALSE ) > 0;
	g_bForcePrimaryBufferFormat		= this->ReadConfig( SECTION_TWEAK, KEY_FORCEPRIMARYBUFFERFORMAT, FALSE ) > 0;;
	g_nPrimaryBufferBits			= this->ReadConfig( SECTION_TWEAK, KEY_PRIMARYBUFFERBITS, 16 );
	g_nPrimaryBufferSamples			= this->ReadConfig( SECTION_TWEAK, KEY_PRIMARYBUFFERSAMPLES, 44100 );
	g_nPrimaryBufferChannels		= this->ReadConfig( SECTION_TWEAK, KEY_PRIMARYBUFFERCHANNELS, 2 );

	g_bForceSpeakerConfig			= this->ReadConfig( SECTION_TWEAK, KEY_FORCESPEAKERCONFIG, FALSE ) > 0;
	g_nSpeakerConfig				= this->ReadConfig( SECTION_TWEAK, KEY_SPEAKERCONFIG, DSSPEAKER_5POINT1 );

	g_bStoppedDriverWorkaround		= this->ReadConfig( SECTION_TWEAK, KEY_WORKAROUNDSTOPPEDDRIVER, FALSE ) > 0;

	//
	// Write back to file (or create file if not existing)
	//

#ifdef ENABLE_LOG
	this->WriteConfig( SECTION_DEBUG, KEY_LOGSYSTEM, g_bLogSystem );
	this->WriteConfig( SECTION_DEBUG, KEY_LOGDIRECTSOUND, g_bLogDirectSound );
	this->WriteConfig( SECTION_DEBUG, KEY_LOGDIRECTSOUNDBUFFER, g_bLogDirectSoundBuffer );
	this->WriteConfig( SECTION_DEBUG, KEY_LOGDIRECTSOUND3DBUFFER, g_bLogDirectSound3DBuffer );
	this->WriteConfig( SECTION_DEBUG, KEY_LOGDIRECTSOUND3DLISTENER, g_bLogDirectSound3DListener );
	this->WriteConfig( SECTION_DEBUG, KEY_DEBUGBEEP, g_bDebugBeep );
#endif //ENABLE_LOG

	this->WriteConfig( SECTION_TWEAK, KEY_NUM2DBUFF, g_nNum2DBuffers );
	this->WriteConfig( SECTION_TWEAK, KEY_NUM3DBUFF, g_nNum3DBuffers );
	this->WriteConfig( SECTION_TWEAK, KEY_FORCECERT, g_bForceCertification );
	this->WriteConfig( SECTION_TWEAK, KEY_FORCEEXCLUSIVEMODE, g_bForceExclusiveMode );
	this->WriteConfig( SECTION_TWEAK, KEY_FORCESOFTMIX, g_bForceSoftwareMixing );
	this->WriteConfig( SECTION_TWEAK, KEY_FORCEHARDMIX, g_bForceHardwareMixing );
	this->WriteConfig( SECTION_TWEAK, KEY_PREVSPEAKSETUP, g_bPreventSpeakerSetup );
	this->WriteConfig( SECTION_TWEAK, KEY_HQSOFT3D, g_bForceHQSoftware3D );
	this->WriteConfig( SECTION_TWEAK, KEY_FORCENONSTATIC, g_bForceNonStatic );
	this->WriteConfig( SECTION_TWEAK, KEY_FORCEVOICEMANAGEMENT, g_bForceVoiceManagement );
	this->WriteConfig( SECTION_TWEAK, KEY_FORCEPRIMARYBUFFERFORMAT, 	g_bForcePrimaryBufferFormat );
	this->WriteConfig( SECTION_TWEAK, KEY_PRIMARYBUFFERBITS, g_nPrimaryBufferBits );
	this->WriteConfig( SECTION_TWEAK, KEY_PRIMARYBUFFERSAMPLES, g_nPrimaryBufferSamples );
	this->WriteConfig( SECTION_TWEAK, KEY_PRIMARYBUFFERCHANNELS, g_nPrimaryBufferChannels );
	this->WriteConfig( SECTION_TWEAK, KEY_FORCESPEAKERCONFIG, g_bForceSpeakerConfig );
	this->WriteConfig( SECTION_TWEAK, KEY_SPEAKERCONFIG, g_nSpeakerConfig);
	this->WriteConfig( SECTION_TWEAK, KEY_WORKAROUNDSTOPPEDDRIVER, g_bStoppedDriverWorkaround );

#ifdef ENABLE_LOG
	g_dwStartTime = ::timeGetTime();

	if( g_bLogSystem == true )
	{
		CString sMessage;
		CTime oTime = CTime::GetCurrentTime();

		sMessage.Format("********* DirectSound Control %s  Logging started at %s *********", PROGRAM_VERSION, oTime.Format("%B %d, %Y %H:%M:%S") );
		::LogMessage( (const char*) NULL, NULL, sMessage.GetBuffer());

		::LogMessage( m_cszClassName, this, "Configuration read....");
	}
#endif //ENABLE_LOG

	if( g_bForceVoiceManagement & ( g_bForceSoftwareMixing | g_bForceHardwareMixing ))
	{
		g_bForceHardwareMixing = false;
		g_bForceSoftwareMixing = false;

#ifdef ENABLE_LOG
		if( g_bLogSystem == true )
			LogMessage( m_cszClassName, this, "ERROR: Voice Management can't be used together with force of hardware/software mixing ....using Voice Management");
#endif // ENABLE_LOG
	}

	if( g_bForceSoftwareMixing & g_bForceHardwareMixing )
	{
		g_bForceHardwareMixing = false;

#ifdef ENABLE_LOG
		if( g_bLogSystem == true )
			LogMessage( m_cszClassName, this, "ERROR: Can't force usage of hardware/software mixing together ....using Software Mixing");
#endif // ENABLE_LOG
	}

	//
	// Get path to original DSOUND.DLL
	//

	char buffer[1024];

	::GetSystemDirectory( buffer, 1024 );
	::PathAddBackslash( buffer );
	strcat_s(buffer, 1024, "dsound.dll" );
	//strcpy_s( buffer + strlen( buffer ), 1024, "dsound.dll" );
  

	//
	// Load DLL and get entry points
	//

	g_hDLL = ::LoadLibrary( buffer );

	if( g_hDLL == NULL )
	{
#ifdef ENABLE_LOG
		if( g_bLogSystem == true )
			LogMessage( m_cszClassName, this, "ERROR: Can't load original DLL...fatal...");
#endif // ENABLE_LOG

		return FALSE;
	}

	g_pDirectSoundCreate				= (DirectSoundCreatefunc)  ::GetProcAddress( g_hDLL, "DirectSoundCreate" );
	g_pDirectSoundCreate8				= (DirectSoundCreate8func) ::GetProcAddress( g_hDLL, "DirectSoundCreate8" );
	g_pGetDeviceIDfunc					= (GetDeviceIDfunc) ::GetProcAddress( g_hDLL, "GetDeviceID" );
	g_pDirectSoundEnumerateAfunc		= (DirectSoundEnumerateAfunc)  ::GetProcAddress( g_hDLL, "DirectSoundEnumerateA" );
	g_pDirectSoundEnumerateWfunc		= (DirectSoundEnumerateWfunc ) ::GetProcAddress( g_hDLL, "DirectSoundEnumerateW" );
	g_pDirectSoundCaptureCreatefunc		= (DirectSoundCaptureCreatefunc ) ::GetProcAddress( g_hDLL, "DirectSoundCaptureCreate" );
	g_pDirectSoundCaptureEnumerateAfunc = (DirectSoundCaptureEnumerateAfunc)  ::GetProcAddress( g_hDLL, "DirectSoundCaptureEnumerateA" );
	g_pDirectSoundCaptureEnumerateWfunc = (DirectSoundCaptureEnumerateWfunc)  ::GetProcAddress( g_hDLL, "DirectSoundCaptureEnumerateW" );
	g_pDirectSoundCaptureCreate8func	= (DirectSoundCaptureCreate8func)  ::GetProcAddress( g_hDLL, "DirectSoundCaptureCreate8" );
	g_pDirectSoundFullDuplexCreatefunc	= (DirectSoundFullDuplexCreatefunc)  ::GetProcAddress( g_hDLL, "DirectSoundFullDuplexCreate" );
	g_pDllGetClassObjectfunc			= (DllGetClassObjectfunc) ::GetProcAddress( g_hDLL, "DllGetClassObject" );
	g_pDllCanUnloadNowfunc				= (DllCanUnloadNowfunc) ::GetProcAddress( g_hDLL, "DllCanUnloadNow" );

#ifdef ENABLE_LOG
	if( g_bDebugBeep == true )
		Beep(1000, 100);

	if( g_bLogSystem == true )
	{
		::LogMessage( m_cszClassName, this, "Original DSOUND.DLL loaded....");
		
		CString sMessage;
		
		sMessage.Format("Location,%s", buffer );
		::LogMessage( m_cszClassName, this, sMessage.GetBuffer());

		sMessage = "OS," + ::GetOSVersion();
		::LogMessage( m_cszClassName, this, sMessage.GetBuffer());

		sMessage = "DirectX," + ::GetDirectXVersionFromReg();
		::LogMessage( m_cszClassName, this, sMessage.GetBuffer());

		::GetCurrentDirectory(255, sMessage.GetBuffer(255));
		sMessage.ReleaseBuffer();
		sMessage = "Working directory," + sMessage;
		::LogMessage( m_cszClassName, this, sMessage.GetBuffer());

		::LogMessage( m_cszClassName, this, "Beginning device enumeration...");
		g_pDirectSoundEnumerateAfunc( DSEnumCallback, this );
		::LogMessage( m_cszClassName, this, "Device enumeration complete...");
	}
#endif //ENABLE_LOG

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// Reads a configuration value from INI File
int CDSoundCtrlApp::ReadConfig( char* szSection, char* szKey, int nDefault )
{
	char cBuffer[256];
	char cDefault[256];

	sprintf_s( cDefault, "%d", nDefault );

	DWORD dwBytes = ::GetPrivateProfileString( szSection, szKey, cDefault, cBuffer, 256, INI_FILE_NAME );

	if( dwBytes == 0 )
	{
		return nDefault;
	}

	return atoi( cBuffer );
}

//////////////////////////////////////////////////////////////////////////////////////////////

// Reads a configuration value from INI File
int CDSoundCtrlApp::WriteConfig( char* szSection, char* szKey, int nVal )
{
	char cBuffer[256];

	sprintf_s( cBuffer, "%d", nVal );

	return  ::WritePrivateProfileString( szSection, szKey, cBuffer, INI_FILE_NAME );
}

//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_LOG
bool CDSoundCtrlApp::EnumCallback( LPGUID  lpGuid,    LPCSTR  lpcstrDescription,  LPCSTR  lpcstrModule )
{
	if( lpGuid != NULL )
	{
		CString sMessage;
		sMessage.Format("Device,%s/%s", lpcstrDescription, lpcstrModule );
		::LogMessage(  m_cszClassName, this, sMessage.GetBuffer());
	}
	return TRUE;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////

DSOUNDCTRL_API HRESULT DirectSoundCreate(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DirectSoundCreate called...");
#endif // ENABLE_LOG

	assert( g_pDirectSoundCreate != NULL );

#ifdef ENABLE_LOG
	if( g_bDebugBeep == true )
		Beep( 4000, 100 );
#endif ENABLE_LOG

	IDirectSound8Ex* pDSX = new IDirectSound8Ex;

	HRESULT hRes = g_pDirectSoundCreate( pcGuidDevice, (LPDIRECTSOUND*)&( pDSX->m_lpDirectSound8 ) /* ppDS8 */ , pUnkOuter );

	if( hRes != S_OK )
	{
		delete pDSX;
		return hRes;
	}

	*ppDS = (LPDIRECTSOUND) pDSX;
	
	return hRes;
}

//////////////////////////////////////////////////////////////////////////////////////////////

BOOL DSDLLEnumCallbackA( LPGUID  lpGuid,    LPCSTR  lpcstrDescription,  LPCSTR  lpcstrModule,   LPVOID  lpContext  )
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
	{
		CString sMessage;
		sMessage.Format("IDirectSoundEnumCallback,%s/%s", lpcstrDescription, lpcstrModule );
		::LogMessage( g_cszClassName, NULL, sMessage.GetBuffer());
	}
#endif // ENABLE_LOG

	return g_pAppDSEnumCallbackA( lpGuid,   lpcstrDescription,  lpcstrModule,   lpContext );
}

//////////////////////////////////////////////////////////////////////////////////////////////

BOOL DSDLLEnumCallbackW(LPGUID  lpGuid,    LPCWSTR  lpcstrDescription,  LPCWSTR  lpcstrModule,   LPVOID  lpContext  )
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
	{
		CString sMessage;
		sMessage.Format("IDirectSoundEnumCallback,%s/%s", lpcstrDescription, lpcstrModule );
		::LogMessage(  g_cszClassName, NULL, sMessage.GetBuffer());
	}
#endif // ENABLE_LOG

	return g_pAppDSEnumCallbackW( lpGuid,   lpcstrDescription,  lpcstrModule,   lpContext );
}

//////////////////////////////////////////////////////////////////////////////////////////////

DSOUNDCTRL_API HRESULT DirectSoundEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DirectSoundEnumerateA called...");
#endif // ENABLE_LOG

	g_pAppDSEnumCallbackA = pDSEnumCallback;

	return g_pDirectSoundEnumerateAfunc( (LPDSENUMCALLBACKA)DSDLLEnumCallbackA, pContext );
}

//////////////////////////////////////////////////////////////////////////////////////////////

DSOUNDCTRL_API HRESULT DirectSoundEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DirectSoundEnumerateW called...");
#endif // ENABLE_LOG

	g_pAppDSEnumCallbackW = pDSEnumCallback;

	return g_pDirectSoundEnumerateWfunc( (LPDSENUMCALLBACKW)DSDLLEnumCallbackW, pContext );
}

//////////////////////////////////////////////////////////////////////////////////////////////

DSOUNDCTRL_API HRESULT DirectSoundCaptureCreate(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE *ppDSC, LPUNKNOWN pUnkOuter)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DirectSoundCaptureCreate called...");
#endif // ENABLE_LOG

	return g_pDirectSoundCaptureCreatefunc( pcGuidDevice, ppDSC, pUnkOuter );
}

//////////////////////////////////////////////////////////////////////////////////////////////

DSOUNDCTRL_API HRESULT DirectSoundCaptureEnumerateA(LPDSENUMCALLBACKA pDSEnumCallback, LPVOID pContext)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DirectSoundCaptureEnumerateA called...");
#endif // ENABLE_LOG

	return g_pDirectSoundCaptureEnumerateAfunc( pDSEnumCallback, pContext );
}

//////////////////////////////////////////////////////////////////////////////////////////////

DSOUNDCTRL_API HRESULT DirectSoundCaptureEnumerateW(LPDSENUMCALLBACKW pDSEnumCallback, LPVOID pContext)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DirectSoundCaptureEnumerateW called...");
#endif // ENABLE_LOG

	return g_pDirectSoundCaptureEnumerateWfunc( pDSEnumCallback, pContext );
}

//////////////////////////////////////////////////////////////////////////////////////////////

DSOUNDCTRL_API HRESULT DirectSoundCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUND8 *ppDS8, LPUNKNOWN pUnkOuter)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DirectSoundCreate8 called...");

	if( g_bDebugBeep == true )
		Beep(2000, 100);
#endif // ENABLE_LOG

	assert( g_pDirectSoundCreate8 != NULL );

	IDirectSound8Ex* pDSX = new IDirectSound8Ex;

	HRESULT hRes = g_pDirectSoundCreate8( pcGuidDevice, &( pDSX->m_lpDirectSound8 ) /* ppDS8 */ , pUnkOuter );

	if( hRes != S_OK )
	{
		delete pDSX;
		return hRes;
	}

	*ppDS8 = (LPDIRECTSOUND8) pDSX;
	
	return hRes;
}

//////////////////////////////////////////////////////////////////////////////////////////////

DSOUNDCTRL_API HRESULT DirectSoundCaptureCreate8(LPCGUID pcGuidDevice, LPDIRECTSOUNDCAPTURE8 *ppDSC8, LPUNKNOWN pUnkOuter)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DirectSoundCaptureCreate8 called...");
#endif // ENABLE_LOG

	return g_pDirectSoundCaptureCreate8func( pcGuidDevice, ppDSC8, pUnkOuter );
}

//////////////////////////////////////////////////////////////////////////////////////////////

DSOUNDCTRL_API HRESULT DirectSoundFullDuplexCreate(LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice,
        LPCDSCBUFFERDESC pcDSCBufferDesc, LPCDSBUFFERDESC pcDSBufferDesc, HWND hWnd,
        DWORD dwLevel, LPDIRECTSOUNDFULLDUPLEX* ppDSFD, LPDIRECTSOUNDCAPTUREBUFFER8 *ppDSCBuffer8,
        LPDIRECTSOUNDBUFFER8 *ppDSBuffer8, LPUNKNOWN pUnkOuter)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DirectSoundFullDuplexCreate called...");
#endif // ENABLE_LOG

	return g_pDirectSoundFullDuplexCreatefunc( pcGuidCaptureDevice, pcGuidRenderDevice,
        pcDSCBufferDesc, pcDSBufferDesc, hWnd,
        dwLevel, ppDSFD, ppDSCBuffer8,
        ppDSBuffer8, pUnkOuter);
}

//////////////////////////////////////////////////////////////////////////////////////////////

DSOUNDCTRL_API HRESULT GetDeviceID(LPCGUID pGuidSrc, LPGUID pGuidDest)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "GetDeviceID called...");
#endif // ENABLE_LOG

	return g_pGetDeviceIDfunc( pGuidSrc, pGuidDest );
}

//////////////////////////////////////////////////////////////////////////////////////////////

STDAPI DllCanUnloadNow(void)
{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DllCanUnloadNow called...");
#endif // ENABLE_LOG

	return g_pDllCanUnloadNowfunc();
}
//////////////////////////////////////////////////////////////////////////////////////////////

STDAPI DllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv)
{
	*ppv = NULL;

	if (( rclsid != CLSID_DirectSound ) && ( rclsid != CLSID_DirectSound8 ))
	{
#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DllGetClassObject with unknown CLSID called...");
#endif // ENABLE_LOG

		return g_pDllGetClassObjectfunc( rclsid, riid, ppv );
	}

	if( riid == IID_IClassFactory )
	{
		IDirectSoundClassFactoryEx* pDSCFX = new IDirectSoundClassFactoryEx;

		HRESULT hRes = g_pDllGetClassObjectfunc( rclsid, riid, (LPVOID*) &(pDSCFX->m_lpClassFactory) );

		if( hRes != NULL )
		{
#ifdef ENABLE_LOG
			if( g_bLogSystem == true )
				LogMessage( g_cszClassName, NULL, "DllGetClassObject for IID_IClassFactory failed...");
#endif // ENABLE_LOG

			delete pDSCFX;
			return hRes;
		}
		
		*ppv = (LPVOID*) pDSCFX;

#ifdef ENABLE_LOG
		if( g_bLogSystem == true )
			LogMessage( g_cszClassName, NULL, "DllGetClassObject for IID_IClassFactory called...");
#endif // ENABLE_LOG

		return S_OK;
	}

#ifdef ENABLE_LOG
	if( g_bLogSystem == true )
		LogMessage( g_cszClassName, NULL, "DllGetClassObject for unknown IID interface called...");
#endif // ENABLE_LOG

	return g_pDllGetClassObjectfunc( rclsid, riid, ppv );
}

//////////////////////////////////////////////////////////////////////////////////////////////

// Dll main function
bool APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(hModule);
	UNREFERENCED_PARAMETER(lpReserved);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		theApp.InitInstance();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return true;
}
