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


// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0400	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

//////////////////////////////////////////////////////////////////////////////////////////////

#include <shlwapi.h>		// For shell functions

//////////////////////////////////////////////////////////////////////////////////////////////

#include <mmreg.h>
#include "mmsystem.h"
#include "dsound.h"
#include "exports.h"

//////////////////////////////////////////////////////////////////////////////////////////////

#define INI_FILE_NAME					".\\dsoundctrl.ini"
#define LOG_FILE_NAME					"dsoundctrl.csv"

#define SECTION_TWEAK					"Tweak"

#ifdef ENABLE_LOG
	#define SECTION_DEBUG					"Debug"

	#define KEY_LOGSYSTEM					"LogSystem"
	#define KEY_LOGDIRECTSOUND				"LogIDirectSound"
	#define KEY_LOGDIRECTSOUNDBUFFER		"LogIDirectSoundBuffer"
	#define KEY_LOGDIRECTSOUND3DBUFFER		"LogIDirectSound3DBuffer"
	#define KEY_LOGDIRECTSOUND3DLISTENER	"LogIDirectSound3DListener"
	#define KEY_DEBUGBEEP					"DebugBeep"
#endif // ENABLE_LOG

#define KEY_NUM2DBUFF					"Num2DBuffers"
#define KEY_NUM3DBUFF					"Num3DBuffers"
#define KEY_FORCECERT					"ForceCertification"
#define KEY_FORCEEXCLUSIVEMODE			"ForceExclusiveMode" 
#define KEY_FORCESOFTMIX				"ForceSoftwareMixing"
#define KEY_FORCEHARDMIX				"ForceHardwareMixing"
#define KEY_PREVSPEAKSETUP				"PreventSpeakerSetup"
#define KEY_HQSOFT3D					"ForceHQ3DSoftMixing"
#define KEY_FORCENONSTATIC				"ForceNonStaticBuffers"
#define KEY_FORCEVOICEMANAGEMENT		"ForceVoiceManagement"
#define KEY_FORCEPRIMARYBUFFERFORMAT	"ForcePrimaryBufferFormat"
#define KEY_PRIMARYBUFFERBITS			"PrimaryBufferBits"
#define KEY_PRIMARYBUFFERSAMPLES		"PrimaryBufferSamples"
#define KEY_PRIMARYBUFFERCHANNELS		"PrimaryBufferChannels"

#define KEY_WORKAROUNDSTOPPEDDRIVER		"EnableStoppedDriverWorkaround"

#define KEY_FORCESPEAKERCONFIG			"ForceSpeakerConfig"
#define KEY_SPEAKERCONFIG				"SpeakerConfig"

//////////////////////////////////////////////////////////////////////////////////////////////

#define PROGRAM_VERSION					"V1.82"

//////////////////////////////////////////////////////////////////////////////////////////////

