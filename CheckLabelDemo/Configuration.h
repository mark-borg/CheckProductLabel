// Configuration.h: interface for the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#define CURRENT_TIMESLOT 1 // ONLY THE FIRST MONTH THEY GET A YEAR FRE UPGRADES
#define MAIN_KEY  _T("State")
#define PRODUCT  _T("CheckLabel")
#define COMPANY  _T("ScanExpress")

class CConfiguration  
{
public:
	BOOL RegDelete(const TCHAR *Section, const TCHAR *Key)
	{
		// Adjust the AppID for Local Server or Service
		CRegKey keyAppID;
		LONG lRes = keyAppID.Open(HKEY_CURRENT_USER, _T("Software"), KEY_WRITE);
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		TCHAR kstr[MAX_PATH];

		_tcscpy(kstr, COMPANY);
		_tcscat(kstr, _T("\\"));
		_tcscat(kstr, PRODUCT);
		_tcscat(kstr, _T("\\"));
		_tcscat(kstr, Section);

		CRegKey key;
		lRes = key.Create(keyAppID, kstr); // If the key does not exist, create it
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		lRes = key.DeleteValue(Key);
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		return TRUE;
	}

	void DeleteGeneral(int id)
	{	CString p;
		p.Format(_T("%d"), id);
		RegDelete(MAIN_KEY, p);
	}

	BOOL RegSet(const TCHAR *Section, const TCHAR *Key, const TCHAR *Value)
	{
		// Adjust the AppID for Local Server or Service
		CRegKey keyAppID;
		LONG lRes = keyAppID.Open(HKEY_CURRENT_USER, _T("Software"), KEY_WRITE);
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		TCHAR kstr[MAX_PATH];

		_tcscpy(kstr, COMPANY);
		_tcscat(kstr, _T("\\"));
		_tcscat(kstr, PRODUCT);
		_tcscat(kstr, _T("\\"));
		_tcscat(kstr, Section);

		CRegKey key;
		lRes = key.Create(keyAppID, kstr); // If the key does not exist, create it
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		key.SetValue(Value, Key);

		return TRUE;
	}

	BOOL RegGet(const TCHAR *Section, const TCHAR *Key, TCHAR *Value)
	{
		// Adjust the AppID for Local Server or Service
		CRegKey key;
		TCHAR kname[MAX_PATH];

		_stprintf(kname, _T("Software\\%s\\%s\\%s"), COMPANY, PRODUCT, Section);

		LONG lRes = key.Open(HKEY_CURRENT_USER, kname, KEY_READ);
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		DWORD BufLen = MAX_PATH;

		Value[0] = '\0';
		lRes = key.QueryValue(Value, (TCHAR *)Key, &BufLen);
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		return TRUE;
	}

	BOOL RegGet(const TCHAR *Section, const TCHAR *Key, DWORD &Value)
	{
		// Adjust the AppID for Local Server or Service
		CRegKey key;
		TCHAR kname[MAX_PATH];

		_stprintf(kname, _T("Software\\%s\\%s\\%s"), COMPANY, PRODUCT, Section);

		LONG lRes = key.Open(HKEY_CURRENT_USER, kname, KEY_READ);
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		lRes = key.QueryValue(Value, (TCHAR *)Key);
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		return TRUE;
	}

	BOOL RegSet(const TCHAR *Section, const TCHAR *Key, DWORD Value)
	{
		// Adjust the AppID for Local Server or Service
		CRegKey keyAppID;
		LONG lRes = keyAppID.Open(HKEY_CURRENT_USER, _T("Software"), KEY_WRITE);
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		TCHAR kstr[MAX_PATH];

		_tcscpy(kstr, COMPANY);
		_tcscat(kstr, _T("\\"));
		_tcscat(kstr, PRODUCT);
		_tcscat(kstr, _T("\\"));
		_tcscat(kstr, Section);

		CRegKey key;
		lRes = key.Create(keyAppID, kstr); // If the key does not exist, create it
		if (lRes != ERROR_SUCCESS)
			return FALSE;

		key.SetValue(Value, Key);
		return TRUE;
	}
	
	int GetProfileInt(const TCHAR *Section, const TCHAR *Key, int def)
	{
		DWORD Value = def;
		CString s = GetProfileString(Section, Key, _T(""));
		if (!s.IsEmpty())
			Value = _ttoi(s);
		return Value;
	}
	
	CString GetProfileString(const TCHAR *Section, const TCHAR *Key, const TCHAR *def)
	{
		TCHAR buf[MAX_PATH];
		CString Value;
		if (!RegGet(Section, Key, buf))
			Value = def;
		else
			Value = buf;
		return Value;
	}
	
	BOOL WriteProfileInt(const TCHAR *Section, const TCHAR *Key, int value)
	{   CString s;
		s.Format(_T("%d"), value);
		return WriteProfileString(Section, Key, s);
	}
	
	BOOL WriteProfileString(const TCHAR *Section, const TCHAR *Key, const TCHAR *value)
	{ return RegSet(Section, Key, value);
	}

	int GetProfileIntEx(const TCHAR *Section, const TCHAR *Key, int def)
	{
		DWORD Value = 0;
		if (!RegGet(Section, Key, Value))
			Value = def;
		return Value;
	}
	
	BOOL WriteProfileIntEx(const TCHAR *Section, const TCHAR *Key, int value)
	{ return RegSet(Section, Key, value);
	}

	DWORD mbIsTrial;
	void SetScreenPos(long x, long y);
	void GetScreenPos(long *x, long *y);
	void Encrypt(const TCHAR* seed, CString &result);

	void GetGeneral(UINT id, CString &s, const TCHAR *def = _T(""));
	void SetGeneral(UINT id, CString &s);

	void GetGeneral(UINT id, int *idx, int def = 0);
	void SetGeneral(UINT id, int idx);

	void GetLastFolder(CString &s);
	void SetLastFolder(CString &s);
//  ONLY FOR KEY GENERATION
//	void PackCode(CString &result, CString &s);
	
	void UnpackCode(const TCHAR* code, CString &seed);
	void InsertTimeSlot(CString &p, CString &result, CString &s);

	void SetWarningBox(UINT d, bool b);
	bool GetWarningBox(UINT d);
	void GetWebsite(CString &s);
//	bool IsLangSupported(WORD lang)
//	{ for (int i = 0; i < SUPPORTED_LANGUAGES && gSubbortedLanguages[i] != lang; i++);
//	  return (i < SUPPORTED_LANGUAGES); }

	CConfiguration();
	virtual ~CConfiguration();
};

int ConvertStringToInt(int base, const TCHAR* input);


#define VERSION_NUMBER		   "1.0.0"
#define START_CONFIG		 1000
#define LAST_TRANSFER_PATH_SRC START_CONFIG + 1
#define REG_CHECK_WMF		   START_CONFIG + 2
#define REG_SEED			   START_CONFIG + 3
#define REG_CODE			   START_CONFIG + 4


#define LAST_CONFIG            START_CONFIG + 100
#define REG_LANGUAGE           2010