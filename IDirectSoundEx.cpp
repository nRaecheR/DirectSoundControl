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
#include <assert.h>
#include "mmsystem.h"
#include "dsound.h"
#include "idirectsoundex.h"
#include "idirectsoundbufferex.h"

//////////////////////////////////////////////////////////////////////////////////////////////

IDirectSound8Ex::IDirectSound8Ex(void)
{
	m_lpDirectSound8 = NULL;

#ifdef ENABLE_LOG
	m_cszClassName = IDIRECTSOUND8EX_CLASS_NAME;

	if( g_bLogDirectSound == true )
		LogMessage( m_cszClassName, this, "Constructor called....");
#endif // ENABLE_LOG

}

//////////////////////////////////////////////////////////////////////////////////////////////

IDirectSound8Ex::~IDirectSound8Ex(void)
{
#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
		LogMessage( m_cszClassName, this, "Destructor called....");
#endif // ENABLE_LOG
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IDirectSound8Ex::QueryInterface( REFIID refIID, LPVOID * pVoid )
{
	*pVoid = NULL;

	if(( refIID == IID_IDirectSound ) || ( refIID == IID_IDirectSound8 ))
	{
		LPVOID pTemp;
		HRESULT hRes =  m_lpDirectSound8->QueryInterface( refIID, &pTemp );

		if( hRes != S_OK )
		{
#ifdef ENABLE_LOG
			if( g_bLogDirectSound == true )
				::LogMessage( m_cszClassName, this, "QueryInterface for interface IID_IDirectSound8 failed...");
#endif // ENABLE_LOG
		
			return hRes;	
		}

		m_lpDirectSound8 = (LPDIRECTSOUND8) pTemp;

		*pVoid = (LPVOID) this;

#ifdef ENABLE_LOG
		if( g_bLogDirectSound == true )
			::LogMessage( m_cszClassName, this, "QueryInterface called,Interface=IID_IDirectSound8");
#endif // ENABLE_LOG

		return S_OK;
	}

#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
		LogMessage( m_cszClassName, this, "QueryInterface called for unknown IID interface ....");
#endif // ENABLE_LOG

	return m_lpDirectSound8->QueryInterface( refIID, pVoid );
}

//////////////////////////////////////////////////////////////////////////////////////////////

ULONG IDirectSound8Ex::AddRef()
{
	ULONG nRefCnt = m_lpDirectSound8->AddRef();

#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
	{
		CString sLogString;
		sLogString.Format("AddRef called....,nRefCnt=%u", nRefCnt ); 

		::LogMessage( m_cszClassName, this, sLogString.GetBuffer());
	}
#endif // ENABLE_LOG

	return nRefCnt;
}

//////////////////////////////////////////////////////////////////////////////////////////////

ULONG IDirectSound8Ex::Release()
{
	ULONG nRefCnt = m_lpDirectSound8->Release();

#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
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

HRESULT IDirectSound8Ex::CreateSoundBuffer    (LPCDSBUFFERDESC pcDSBufferDesc, LPDIRECTSOUNDBUFFER *ppDSBuffer, LPUNKNOWN pUnkOuter) 
{
	DSBUFFERDESC dsdesc = *pcDSBufferDesc;

#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
	{
		CString sLogString;
		CString sTempString;
		sLogString = "CreateSoundBuffer called,";

		sTempString.Format("dwBufferBytes=%u,Flags=", pcDSBufferDesc->dwBufferBytes );
		sLogString += sTempString;

		if( pcDSBufferDesc->dwFlags & DSBCAPS_PRIMARYBUFFER  )
			sLogString += "DSBCAPS_PRIMARYBUFFER";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_CTRL3D )
			sLogString += "|DSBCAPS_CTRL3D";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_STATIC   )
			sLogString += "|DSBCAPS_STATIC";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_LOCDEFER  )
			sLogString += "|DSBCAPS_LOCDEFER";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_LOCHARDWARE  )
			sLogString += "|DSBCAPS_LOCHARDWARE";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_LOCSOFTWARE  )
			sLogString += "|DSBCAPS_LOCSOFTWARE";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_CTRLFREQUENCY  )
			sLogString += "|DSBCAPS_CTRLFREQUENCY";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_CTRLPAN )
			sLogString += "|DSBCAPS_CTRLPAN";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_CTRLPOSITIONNOTIFY  )
			sLogString += "|DSBCAPS_CTRLPOSITIONNOTIFY";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_CTRLVOLUME   )
			sLogString += "|DSBCAPS_CTRLVOLUME";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_GETCURRENTPOSITION2  )
			sLogString += "|DSBCAPS_GETCURRENTPOSITION2";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_GLOBALFOCUS   )
			sLogString += "|DSBCAPS_GLOBALFOCUS";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_MUTE3DATMAXDISTANCE  )
			sLogString += "|DSBCAPS_MUTE3DATMAXDISTANCE";
		if( pcDSBufferDesc->dwFlags & DSBCAPS_STICKYFOCUS   )
			sLogString += "|DSBCAPS_STICKYFOCUS";

		if(( pcDSBufferDesc->dwFlags & DSBCAPS_CTRL3D ) && (pcDSBufferDesc->dwFlags & DSBCAPS_LOCSOFTWARE) )
		{
			sLogString += ",3DALGORITHM=";

			if( pcDSBufferDesc->guid3DAlgorithm == DS3DALG_DEFAULT )
				sLogString += "DS3DALG_DEFAULT";
			if( pcDSBufferDesc->guid3DAlgorithm == DS3DALG_NO_VIRTUALIZATION )
				sLogString += "DS3DALG_NO_VIRTUALIZATION";
			if( pcDSBufferDesc->guid3DAlgorithm == DS3DALG_HRTF_FULL )
				sLogString += "DS3DALG_HRTF_FULL";
			if( pcDSBufferDesc->guid3DAlgorithm == DS3DALG_HRTF_LIGHT )
				sLogString += "DS3DALG_HRTF_LIGHT";
		}

		if( pcDSBufferDesc->lpwfxFormat != NULL )
		{
			sTempString.Format(",Format=%ub %uhz %uch", pcDSBufferDesc->lpwfxFormat->wBitsPerSample, pcDSBufferDesc->lpwfxFormat->nSamplesPerSec, pcDSBufferDesc->lpwfxFormat->nChannels );
			sLogString += sTempString;
		}

		::LogMessage( m_cszClassName, this, sLogString.GetBuffer());
	}
#endif // ENABLE_LOG

	if( g_bForceSoftwareMixing == true )
	{
#ifdef ENABLE_LOG
		if( g_bLogDirectSound == true )
			::LogMessage( m_cszClassName, this, "CreateSoundBuffer FORCING SOFTWARE MIXED BUFFER....");
#endif // ENABLE_LOG

		dsdesc.dwFlags &= ~DSBCAPS_LOCHARDWARE;
		dsdesc.dwFlags |= DSBCAPS_LOCSOFTWARE;
	}

	if( g_bForceHardwareMixing == true )
	{
		
#ifdef ENABLE_LOG
		if( g_bLogDirectSound == true )
			::LogMessage( m_cszClassName, this, "CreateSoundBuffer FORCING HARDWARE MIXED BUFFER....");
#endif // ENABLE_LOG

		dsdesc.dwFlags &= ~DSBCAPS_LOCSOFTWARE;
		dsdesc.dwFlags |= DSBCAPS_LOCHARDWARE;
	}

	if(( g_bForceHQSoftware3D == true ) && (dsdesc.dwFlags & DSBCAPS_LOCSOFTWARE) && (dsdesc.dwFlags & DSBCAPS_CTRL3D))
	{
		dsdesc.guid3DAlgorithm = DS3DALG_HRTF_FULL;

#ifdef ENABLE_LOG
		if( g_bLogDirectSound == true )
			::LogMessage( m_cszClassName, this, "CreateSoundBuffer FORCING HQ 3D SOFTWARE PROCESSING....");
#endif // ENABLE_LOG
	}

	if(( g_bForceNonStatic == true ) && ( dsdesc.dwFlags & DSBCAPS_STATIC ))
	{
#ifdef ENABLE_LOG
		if( g_bLogDirectSound == true )
			::LogMessage( m_cszClassName, this, "CreateSoundBuffer FORCING NON-STATIC BUFFER....");
#endif // ENABLE_LOG

		dsdesc.dwFlags &= ~DSBCAPS_STATIC;
	}

	if(( g_bForceVoiceManagement == true ) && (( dsdesc.dwFlags & DSBCAPS_LOCDEFER ) == 0 ))
	{
#ifdef ENABLE_LOG
		if( g_bLogDirectSound == true )
			::LogMessage(m_cszClassName, this, "CreateSoundBuffer FORCING BUFFER MANAGMEMENT....");
#endif // ENABLE_LOG

		dsdesc.dwFlags &= ~DSBCAPS_LOCSOFTWARE;
		dsdesc.dwFlags &= ~DSBCAPS_LOCHARDWARE;
		dsdesc.dwFlags |= DSBCAPS_LOCDEFER;
	}

	IDirectSoundBuffer8Ex* pDSBX = new IDirectSoundBuffer8Ex;

	HRESULT hRes = m_lpDirectSound8->CreateSoundBuffer( &dsdesc, (LPDIRECTSOUNDBUFFER*) &( pDSBX->m_lpDirectSoundBuffer8 ) /*ppDSBuffer*/,  pUnkOuter ) ;

	*ppDSBuffer = (LPDIRECTSOUNDBUFFER8) pDSBX;

	if( pcDSBufferDesc->dwFlags & DSBCAPS_PRIMARYBUFFER )
	{
		pDSBX->SetPrimaryBuffer( true );

		if((hRes == S_OK) && ( g_bForcePrimaryBufferFormat == true ))
		{

	#ifdef ENABLE_LOG
			if( g_bLogDirectSound == true )
			{
				CString sMessage;
				sMessage.Format("FORCING PRIMARY BUFFER FORMAT,Format=%ub %uhz %uch", g_nPrimaryBufferBits,g_nPrimaryBufferSamples,g_nPrimaryBufferChannels );

				::LogMessage(m_cszClassName, this, sMessage.GetBuffer() );
			}
	#endif // ENABLE_LOG

			WAVEFORMATEX fxFormat;
			fxFormat.wFormatTag		=	WAVE_FORMAT_PCM;
			fxFormat.cbSize			=	0;
			fxFormat.nChannels		=	(WORD) g_nPrimaryBufferChannels;
			fxFormat.wBitsPerSample	=	(WORD) g_nPrimaryBufferBits;
			fxFormat.nSamplesPerSec	=	g_nPrimaryBufferSamples;
			fxFormat.nBlockAlign	=	(fxFormat.nChannels * fxFormat.wBitsPerSample) / 8;
			fxFormat.nAvgBytesPerSec=	fxFormat.nBlockAlign * fxFormat.nSamplesPerSec;
			
			HRESULT hResult = pDSBX->m_lpDirectSoundBuffer8->SetFormat	( &fxFormat );
	#ifdef ENABLE_LOG
			if(( g_bLogDirectSound == true ) && ( hResult != S_OK ))
				::LogMessage(m_cszClassName, this, "ERROR: FORCING PRIMARY BUFFER FORMAT failed....");
	#else
			UNREFERENCED_PARAMETER(hResult);
	#endif // ENABLE_LOG
		}
	}

	return hRes;
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IDirectSound8Ex::GetCaps              (LPDSCAPS pDSCaps)
{
#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
		::LogMessage(m_cszClassName, this, "GetCaps called....");
#endif // ENABLE_LOG

	HRESULT hRes = m_lpDirectSound8->GetCaps( pDSCaps );

	if( g_nNum2DBuffers	!=	0 )
	{
		if( pDSCaps->dwMaxHwMixingAllBuffers >= (DWORD) g_nNum2DBuffers )
		{
			assert( g_nNum2DBuffers >= (int)( pDSCaps->dwMaxHwMixingAllBuffers - pDSCaps->dwFreeHwMixingAllBuffers ));
			assert( g_nNum2DBuffers >= (int)( pDSCaps->dwMaxHwMixingStaticBuffers		-	pDSCaps->dwFreeHwMixingStaticBuffers ));
			assert( g_nNum2DBuffers >= (int)( pDSCaps->dwFreeHwMixingStreamingBuffers	-	pDSCaps->dwMaxHwMixingStreamingBuffers ));

			pDSCaps->dwFreeHwMixingAllBuffers		=	g_nNum2DBuffers - ( pDSCaps->dwMaxHwMixingAllBuffers		-	pDSCaps->dwFreeHwMixingAllBuffers );
			pDSCaps->dwFreeHwMixingStaticBuffers	=	g_nNum2DBuffers - ( pDSCaps->dwMaxHwMixingStaticBuffers		-	pDSCaps->dwFreeHwMixingStaticBuffers );
			pDSCaps->dwFreeHwMixingStreamingBuffers =	g_nNum2DBuffers - ( pDSCaps->dwFreeHwMixingStreamingBuffers	-	pDSCaps->dwMaxHwMixingStreamingBuffers );
			pDSCaps->dwMaxHwMixingAllBuffers		=	g_nNum2DBuffers;
			pDSCaps->dwMaxHwMixingStaticBuffers		=	g_nNum2DBuffers;
			pDSCaps->dwMaxHwMixingStreamingBuffers	=	g_nNum2DBuffers;

#ifdef ENABLE_LOG
			if( g_bLogDirectSound == true )
				::LogMessage( m_cszClassName, this, "GetCaps number of 2D buffers changed....");
#endif // ENABLE_LOG

		}
		else
		{
#ifdef ENABLE_LOG
			if( g_bLogDirectSound == true )
				LogMessage( m_cszClassName, this, "ERROR: GetCaps INVALID number of 2D buffers given, no change....");
#endif // ENABLE_LOG
		}
	}
	
	if( g_nNum3DBuffers	!=	0 )
	{
		if( pDSCaps->dwMaxHw3DAllBuffers >= (DWORD) g_nNum3DBuffers )
		{
			assert( g_nNum3DBuffers >= (int)( pDSCaps->dwMaxHw3DAllBuffers - pDSCaps->dwFreeHw3DAllBuffers ));
			assert( g_nNum3DBuffers >= (int)( pDSCaps->dwMaxHw3DStaticBuffers		-	pDSCaps->dwFreeHw3DStaticBuffers ));
			assert( g_nNum3DBuffers >= (int)( pDSCaps->dwFreeHw3DStreamingBuffers	-	pDSCaps->dwMaxHw3DStreamingBuffers ));

			pDSCaps->dwFreeHw3DAllBuffers			=	g_nNum3DBuffers - ( pDSCaps->dwMaxHw3DAllBuffers		-	pDSCaps->dwFreeHw3DAllBuffers );
			pDSCaps->dwFreeHw3DStaticBuffers		=	g_nNum3DBuffers - ( pDSCaps->dwMaxHw3DStaticBuffers		-	pDSCaps->dwFreeHw3DStaticBuffers );
			pDSCaps->dwFreeHw3DStreamingBuffers		=	g_nNum3DBuffers - ( pDSCaps->dwFreeHw3DStreamingBuffers	-	pDSCaps->dwMaxHw3DStreamingBuffers );
			pDSCaps->dwMaxHw3DAllBuffers			=	g_nNum3DBuffers;
			pDSCaps->dwMaxHw3DStaticBuffers			=	g_nNum3DBuffers;
			pDSCaps->dwMaxHw3DStreamingBuffers		=	g_nNum3DBuffers;

#ifdef ENABLE_LOG
			if( g_bLogDirectSound == true )
				::LogMessage( m_cszClassName, this, "GetCaps number of 3D buffers changed....");
#endif // ENABLE_LOG
		}
		else
		{
#ifdef ENABLE_LOG
			if( g_bLogDirectSound == true )
				::LogMessage( m_cszClassName, this, "ERROR: GetCaps INVALID number of 3D buffers given, no change....");
#endif // ENABLE_LOG
		}
	}

	return hRes; 
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IDirectSound8Ex::DuplicateSoundBuffer (LPDIRECTSOUNDBUFFER pDSBufferOriginal, LPDIRECTSOUNDBUFFER *ppDSBufferDuplicate)
{
#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
	{
		CString sLogString;
		sLogString.Format("DuplicateSoundBuffer called....,pDSBufferOriginal=0x%x,ppDSBufferDuplicate=0x%x",pDSBufferOriginal,ppDSBufferDuplicate );

		::LogMessage( m_cszClassName, this, sLogString.GetBuffer());
	}
#endif // ENABLE_LOG

	*ppDSBufferDuplicate = NULL;

	IDirectSoundBuffer8Ex* pDSBX = new IDirectSoundBuffer8Ex;

	HRESULT hRes = m_lpDirectSound8->DuplicateSoundBuffer( (LPDIRECTSOUNDBUFFER) (((IDirectSoundBuffer8Ex*) pDSBufferOriginal)->m_lpDirectSoundBuffer8 ), (LPDIRECTSOUNDBUFFER*) &( pDSBX->m_lpDirectSoundBuffer8 ) ) ;

	if( hRes != S_OK )
	{
		delete pDSBX;
		return hRes;
	}
	
	*ppDSBufferDuplicate = (LPDIRECTSOUNDBUFFER8) pDSBX;
		
	return hRes;
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IDirectSound8Ex::SetCooperativeLevel  (HWND hwnd, DWORD dwLevel)
{
#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
	{
		switch (dwLevel )
		{
			case DSSCL_EXCLUSIVE : ::LogMessage( m_cszClassName, this, "SetCooperativeLevel called,Level=DSSCL_EXCLUSIVE"); break;
			case DSSCL_NORMAL : ::LogMessage( m_cszClassName, this, "SetCooperativeLevel called,Level=DSSCL_NORMAL"); break;
			case DSSCL_PRIORITY :  ::LogMessage( m_cszClassName, this, "SetCooperativeLevel called,Level=DSSCL_PRIORITY"); break;
			case DSSCL_WRITEPRIMARY : ::LogMessage( m_cszClassName, this, "SetCooperativeLevel called,Level=DSSCL_WRITEPRIMARY"); break;
		}
	}
#endif // ENABLE_LOG

	if( dwLevel == DSSCL_NORMAL )
	{
		if ( g_bForceExclusiveMode == true )
		{
			dwLevel = DSSCL_EXCLUSIVE;

#ifdef ENABLE_LOG
			if( g_bLogDirectSound == true )
				::LogMessage( m_cszClassName, this, "SetCooperativeLevel FORCING EXCLUSIVE MODE ....");
#endif // ENABLE_LOG
		}
	}

	return m_lpDirectSound8->SetCooperativeLevel( hwnd, dwLevel );
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IDirectSound8Ex::Compact()
{
#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
		::LogMessage( m_cszClassName, this, "Compact called....");
#endif // ENABLE_LOG

	return m_lpDirectSound8->Compact();
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IDirectSound8Ex::GetSpeakerConfig     (LPDWORD pdwSpeakerConfig)
{
#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
		::LogMessage( m_cszClassName, this, "GetSpeakerConfig called....");
#endif // ENABLE_LOG

	if( g_bForceSpeakerConfig == true )
	{
#ifdef ENABLE_LOG
		if( g_bLogDirectSound == true )
			::LogMessage( m_cszClassName, this, "GetSpeakerConfig FORCED....");
#endif  // ENABLE_LOG
	
		*pdwSpeakerConfig = g_nSpeakerConfig;

		return S_OK;
	}


	return m_lpDirectSound8->GetSpeakerConfig( pdwSpeakerConfig );
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IDirectSound8Ex::SetSpeakerConfig     (DWORD dwSpeakerConfig)
{
#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
	{
		CString sLogString;
		sLogString = "SetSpeakerConfig called....,dwSpeakerConfig=";

		if( dwSpeakerConfig == DSSPEAKER_DIRECTOUT )		sLogString += "DSSPEAKER_DIRECTOUT";
		if( dwSpeakerConfig == DSSPEAKER_HEADPHONE  )		sLogString += "DSSPEAKER_HEADPHONE";
		if( dwSpeakerConfig == DSSPEAKER_MONO )				sLogString += "DSSPEAKER_MONO";
		if( dwSpeakerConfig == DSSPEAKER_QUAD )				sLogString += "DSSPEAKER_QUAD";
		if( dwSpeakerConfig == DSSPEAKER_STEREO )			sLogString += "DSSPEAKER_STEREO";
		if( dwSpeakerConfig == DSSPEAKER_SURROUND )			sLogString += "DSSPEAKER_SURROUND";
		if( dwSpeakerConfig == DSSPEAKER_5POINT1 )			sLogString += "DSSPEAKER_5POINT1";
		if( dwSpeakerConfig == DSSPEAKER_7POINT1 )			sLogString += "DSSPEAKER_7POINT1";
		if( dwSpeakerConfig == DSSPEAKER_7POINT1_SURROUND ) sLogString += "DSSPEAKER_7POINT1_SURROUND";

		::LogMessage( m_cszClassName, this, sLogString.GetBuffer());
	}
#endif // ENABLE_LOG

	if( g_bPreventSpeakerSetup == true )
	{
#ifdef ENABLE_LOG
		if( g_bLogDirectSound == true )
			::LogMessage( m_cszClassName, this, "SetSpeakerConfig PREVENTED....");
#endif  // ENABLE_LOG
	
		return S_OK;
	}

	return m_lpDirectSound8->SetSpeakerConfig( dwSpeakerConfig );
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT IDirectSound8Ex::Initialize(LPCGUID pcGuidDevice) 
{
#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
		::LogMessage( m_cszClassName, this, "Initialize called....");
#endif // ENABLE_LOG
	
	return m_lpDirectSound8->Initialize( pcGuidDevice ) ;
}

//////////////////////////////////////////////////////////////////////////////////////////////

HRESULT  IDirectSound8Ex::VerifyCertification(LPDWORD pdwCertified)
{
#ifdef ENABLE_LOG
	if( g_bLogDirectSound == true )
		::LogMessage( m_cszClassName, this, "VerifyCertification called....");
#endif // ENABLE_LOG

	HRESULT hRes = m_lpDirectSound8->VerifyCertification( pdwCertified ) ;

	if( g_bForceCertification == true )
	{
		*pdwCertified = DS_CERTIFIED;

#ifdef ENABLE_LOG
		if( g_bLogDirectSound == true )
			::LogMessage( m_cszClassName, this, "VerifyCertification WHQL certification FORCED....");
#endif // ENABLE_LOG
	
		return S_OK;
	}

	return hRes;
}

//////////////////////////////////////////////////////////////////////////////////////////////
