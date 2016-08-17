// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning (disable : 4996)
#pragma warning (disable : 4786)

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>
#include <atlbase.h>

#include "htmlhelp.h"

///MB:#include "../qvtools_all/qvcore/framework.h"
#include "../qvtools/qvcore/framework.h"
#include "configuration.h"
///MB:#include <_highgui.h>


extern FILE* g_hFileHandle;
extern bool  g_bLoggingEnabled;
extern wchar_t mSeed[40];
extern char	 mCode[100];
extern char log_file_name[MAX_PATH];

extern UINT ShowWarningInfo(CWnd* wnd, UINT id, CString &s);
extern CConfiguration gConfiguration;
extern int gDidgitizedAspect;
extern void GetAppPath(CString &s);
extern void GetAppPath(char* AppPath, char* s);
extern void DisplayHelp(const TCHAR *fname);

extern CString GetLanguageString(UINT id);
extern void LoggingTraceS(const char* mess);
extern void LoggingTraceN1(const char* mess, int i);
extern void LoggingTraceN3(const char* mess, int i, int j, int k);
extern void GetScreenMetrics(int scr, unsigned int *width, unsigned int *height);

#define	CHECK_IF_ERR(x, y, f) \
	if(x) \
	{ \
		AfxMessageBox(TEXT(y), MB_TOPMOST); \
		if (f) GotoDlgItem(f); \
		return false; \
	}

#define	CHECK_IF_ERR_V(x, y, f) \
	if(x) \
	{ \
		AfxMessageBox(TEXT(y), MB_TOPMOST); \
		if (f) GotoDlgItem(f); \
		return; \
	}


// if enabled, a trace file called d:/trace.txt will be created
///#define SAVE_TRACE_LOG


extern char *LoadStateResourceA(UINT id);
extern WCHAR *LoadStateResourceW(UINT id);
extern void LoadLanguageResource(const TCHAR* lang);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

