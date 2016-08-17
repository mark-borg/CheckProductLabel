// stdafx.cpp : source file that includes just the standard includes
//	checklabel.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

CConfiguration gConfiguration;
int gDidgitizedAspect;

wchar_t mSeed[40];
char mCode[100];
char log_file_name[MAX_PATH] = "";

FILE* g_hFileHandle = 0;
bool  g_bLoggingEnabled = false;

void GetAppPath(char* AppPath, char* s)
{
	GetModuleFileNameA(NULL, AppPath, MAX_PATH);
	
	char* pp = (strrchr(AppPath, '\\') + 1);

	if (pp)
		*pp = 0;

	strcat(AppPath, s);
}

void GetAppPath(CString &s)
{
	CString tt;
	TCHAR AppPath[MAX_PATH];
	GetModuleFileName(NULL, AppPath, MAX_PATH);
	tt = AppPath;
	tt = tt.Left(tt.ReverseFind(_T('\\'))+1);
	s = tt + s;
}

UINT ShowWarningInfo(CWnd* wnd, UINT id, CString &mess)
{
//	CTipDlg dlg(wnd, id);
//	if (dlg.m_bStartup)
//	{
//		dlg.m_strTip = mess;
//		return dlg.DoModal();
//	}
	return IDCANCEL;
}

void LoggingTraceS(const char* mess)
{
	if (!g_bLoggingEnabled) return;
	if (!g_hFileHandle)
		g_hFileHandle = fopen(log_file_name, "w+");
	
	fwrite(mess, 1, strlen(mess), g_hFileHandle);
}

void LoggingTraceN1(const char* mess, int i)
{
	char s[1024] = "";

	if (!g_bLoggingEnabled) return;

	if (!g_hFileHandle)
		g_hFileHandle = fopen(log_file_name, "w+");
	
	sprintf(s, mess, i);
	fwrite(s, 1, strlen(s), g_hFileHandle);
}

void LoggingTraceN3(const char* mess, int i, int j, int k)
{
	char s[1024] = "";

	if (!g_bLoggingEnabled) return;

	if (!g_hFileHandle)
		g_hFileHandle = fopen(log_file_name, "w+");
	
	sprintf(s, mess, i, j, k);
	fwrite(s, 1, strlen(s), g_hFileHandle);
}


void DisplayHelp(const TCHAR *fname)
{
	CString s;
	GetAppPath(s);
	s += _T("CheckLabel.chm");

	if (fname)
	{
		s += _T("::/");
		s+=fname;
	}

	HtmlHelp(0, s, HH_DISPLAY_TOC, NULL);
}

void GetScreenMetrics(int scr, unsigned int *width, unsigned int *height)
{
	switch (scr)
	{
		case 1: *width = 220, *height = 176; break;
		case 2: *width = 272, *height = 208; break;
		case 3: *width = 240, *height = 160; break;
		case 4: *width = 160, *height = 128; break;
		case 5: *width = 160, *height = 160; break;
		case 6: *width = 288, *height = 224; break;
		case 7: *width = 304, *height = 240; break;
		case 8: *width = 400, *height = 304; break;
		case 9: *width = 432, *height = 336; break;
		case 10: *width = 464, *height = 352; break;
		case 11: *width = 480, *height = 368; break;
		case 12: *width = 320, *height = 192; break;
		case 13: *width = 448, *height = 256; break;
		case 14: *width = 432, *height = 272; break;
		case 15: *width = 400, *height = 304; break;
		case 16: *width = 640, *height = 480; break;
		default:
		case 0: *width = 320, *height = 240; break;
	}


//	int w = *width, h = *height;
//	if (w%8)
//	{
//		w += (*width%8);
//		*width = w;
//	}
//	if (h%8)
//	{
//		h += (*height%8);
//		*height = h;
//	}
}

CString GetLanguageString(UINT id)
{
	CString s;
	s.LoadString(id);
	return s;
}

void LoadLanguageResource(const TCHAR* lang)
{
	CString langdll;
	langdll.Format(_T("%s.dll"), lang);
	GetAppPath(langdll);
	HINSTANCE hDLL = LoadLibrary(langdll);
	if (hDLL)
		AfxSetResourceHandle(hDLL);
}

WCHAR *LoadStateResourceW(UINT id)
{
	CString s;
	s.Format(_T("#%d"), id);
	// Locate the dialog box resource in the .EXE file. 
	HRSRC hRes = FindResource(GetModuleHandle(NULL), s, _T("BINARY")); 
	if (hRes == NULL) 
	{ 
		return 0;
	} 
 
	// Load the dialog box into global memory. 
	HRSRC hResLoad = (HRSRC)LoadResource(GetModuleHandle(NULL), hRes); 
	if (hResLoad == NULL) 
	{ 
		return 0;
	} 
 
	// Lock the dialog box into global memory. 
	WCHAR *lpResLock = (WCHAR*)LockResource(hResLoad); 
	if (lpResLock == NULL) 
	{ 
		return 0;
	}
	return lpResLock;
}


char *LoadStateResourceA(UINT id)
{
	CString s;
	s.Format(_T("#%d"), id);
	// Locate the dialog box resource in the .EXE file. 
	HRSRC hRes = FindResource(GetModuleHandle(NULL), s, _T("BINARY")); 
	if (hRes == NULL) 
	{ 
		return 0;
	} 
 
	// Load the dialog box into global memory. 
	HRSRC hResLoad = (HRSRC)LoadResource(GetModuleHandle(NULL), hRes); 
	if (hResLoad == NULL) 
	{ 
		return 0;
	} 
 
	// Lock the dialog box into global memory. 
	char *lpResLock = (char*)LockResource(hResLoad); 
	if (lpResLock == NULL) 
	{ 
		return 0;
	} 
	return lpResLock;
}

