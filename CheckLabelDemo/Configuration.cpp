// Configuration.cpp: implementation of the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/* Valid input integers */
#define VALID_DIGITS "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

/*
   Converts an integer to it's equivalent character representation

   input:  int  input - integer to convert
   output: char       - character representation of integer
*/
TCHAR ConvertValueToChar(int input)
{
   if (input < 0 || input > 35)
   {
      /*
         If invalid return a space character since space is
         skipped.
      */
      return 32;
   }
   return VALID_DIGITS[input];
}

/*
   Converts a long value to a string representation based on base

   input:  int base     - base value used for conversion
           unsigned long input
		                - long value to convert
           char* output - destination buffer for converted value
           size_t size  - size of output buffer
   output: char*        - pointer to the output string value
*/
TCHAR* ConvertIntToString(int base, unsigned long input, TCHAR* output,
                         size_t size)
{
   unsigned long quotient = input;
   int remainder = 0;
   TCHAR *marker = output, *start;
   TCHAR first;

   if (!size || output == NULL)
      return NULL;
   
   memset(output, 0, size * sizeof(TCHAR));
   
   if (!input)
      return NULL;

   /*
      to convert back to string divide the quotient by the required base
      then take the remainder and use as an index to the table of valid
      integer representations
   */
   do
   {
      remainder = quotient % base;
      quotient /= base;
      *(marker++) = ConvertValueToChar(remainder);
   } while ((_tcslen(output) < size) && quotient);

   /* Now reverse the string */
   for (start = output, --marker; (start - marker) < 0; start++, marker--)
   {
      first = *start;
      *start  = *marker;
      *marker = first;
   }

   return output;
}

/*
   Converts a single char to it's equivalent numerical
   value.
 
   input:  char input  - char to convert
   output: int         - numerical representation
*/
int ConvertCharToValue(int input)
{
   TCHAR validDigits[] = _T(VALID_DIGITS);

   /* get address of valid integer in list */
   TCHAR* charPos = (TCHAR*)_tcschr(validDigits, input);

   /* not found, invalid */
   if (charPos == NULL)
      return 0;

   /*
      the numerical value represented by this character is the difference
      between the address of the character in the list and the address
      of the begining of the list
   */
   return (charPos - validDigits);
}


/*
   Converts a string representation based on base to its
   equivalent numerical value

   input:  int base          - base value used for conversion
           const char* input - the converted string value
   output: int               - the converted numerical value
*/
int ConvertStringToInt(int base, const TCHAR* input)
{
   unsigned long total = 0;
   const TCHAR* marker;
   if (input == NULL) return 0;

   /*
      The value is obtained by, starting with zero, and then, working
      from left to right, for each digit multiplying by the base and
      adding the value of the digit (2.4)
   */
   for (marker = input; *marker; marker++)
   {
      total *= base;
      total += ConvertCharToValue(*marker);
   }
   return total;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfiguration::CConfiguration()
{
	GetAppPath(log_file_name, "CheckLabel.log");
}

CConfiguration::~CConfiguration()
{

}

void CConfiguration::GetScreenPos(long *x, long *y)
{
	int middlx = 100;
	int middly = (GetSystemMetrics(SM_CYSCREEN)/2) - 170;
	*x = GetProfileIntEx(MAIN_KEY, _T("positionX"), -26);
	*y = GetProfileIntEx(MAIN_KEY, _T("positionY"), -26);
	if (*x < -25 || *x > GetSystemMetrics(SM_CXSCREEN))
		*x = middlx;
	if (*y < -25 || *y > GetSystemMetrics(SM_CYSCREEN))
		*y = middly;
	if (*y < 0) *y = 0;
}

void CConfiguration::SetScreenPos(long x, long y)
{
	WriteProfileIntEx(MAIN_KEY, _T("positionX"), x);
	WriteProfileIntEx(MAIN_KEY, _T("positionY"), y);
}

void CConfiguration::GetGeneral(UINT id, int *idx, int def)
{	CString p;
	p.Format(_T("%d"), id);
	*idx = GetProfileInt(MAIN_KEY, p, def);
}

void CConfiguration::SetGeneral(UINT id, int idx)
{	CString p;
	p.Format(_T("%d"), id);
	WriteProfileInt(MAIN_KEY, p, idx);
}

void CConfiguration::GetGeneral(UINT id, CString &s, const TCHAR *def)
{	CString p, df;
	p.Format(_T("%d"), id);
	s = GetProfileString(MAIN_KEY, p, def);
}

void CConfiguration::SetGeneral(UINT id, CString &s)
{	CString p;
	p.Format(_T("%d"), id);
	WriteProfileString(MAIN_KEY, p, s);
}

void CConfiguration::SetWarningBox(UINT d, bool b)
{
	CString s;
	s.Format(_T("%d"), d);
	WriteProfileInt(MAIN_KEY, s, b ? 1 : 0);
}

bool CConfiguration::GetWarningBox(UINT d)
{
	CString s;
	s.Format(_T("%d"), d);
	return (GetProfileInt(MAIN_KEY, s, 1) ? true : false);
}

//                         25&26
//       m          y      dd     x
//01234567890123456789012345678901234567
//{378ED815-6FD2-11D0-C60B-00A0C90A43CE}
//HKEY_LOCAL_MACHINE\SOFTWARE\Classes\hEpwordForm.Brush\CLSID

/*
void CConfiguration::PackCode(CString &result, CString &s)
{
	int l = result.GetLength();
	s.Empty();
	char buffer[255];
	for (int i = 0; i < l; i+=9)
	{
		if (l-i<9)
			part = result.Mid(i);
		else
			part = result.Mid(i, 9);
		ConvertIntToString(36, atol(part), buffer, sizeof(buffer));
		s+=buffer;
	}

	if (s.GetLength() > 25)
		s = s.Left(25);
}
*/

void CConfiguration::UnpackCode(const TCHAR* code, CString &seed)
{
	CString s = code, p, part;
	int l = s.GetLength(), factor = 0;

	for (int i = 0; i < l; i+=6)
	{
		if (l-i<6)
			part = s.Mid(i);
		else
			part = s.Mid(i, 6);
		factor = ConvertStringToInt(36, part);
		part.Format(_T("%d"), factor);
		p+=part;
	}
	seed = p;
}

bool blackListed(const TCHAR* seed)
{
	CString s;
	int Val11 = 770970460,
		Val12 = 320821111,
		Val13 = 15097114,
		Val14 = 105111032,
		Val15 = 10010110,
		Val16 = 809703206,
		Val17 = 71011141,
		Val18 = 10097;

	s.Format(_T("0%d%d%d%d%d%d%d%d"), Val11, Val12, Val13, Val14, Val15, Val16,
								Val17, Val18);
	CString softKey;
	int len = s.GetLength();
	for (int i = 0; i < len; i+=3)
		softKey += (TCHAR)_ttoi(s.Mid(i,3));
	return (softKey == seed);
}

void CConfiguration::InsertTimeSlot(CString &p, CString &result, CString &s)
{
	p.Format(_T("%d"), CURRENT_TIMESLOT);
	s = result.Left(2) + p + result.Mid(2);
	result = s;
}

void CConfiguration::Encrypt(const TCHAR* seed, CString &result)
{
	if (blackListed(seed)) return;
	
	result.Empty();
	int PARTS = 2;
	int MAX_FACTOR = 5;
	
	CString s, p, part, sSeed = seed;
	int len = sSeed.GetLength();
	int last_factor;
	
	if (_tcslen(seed) < 8) return;

	for (int x = 0; x < len; x+=PARTS)
	{
		part = sSeed.Mid(x, PARTS);
		int part_len = part.GetLength();
		p.Empty();

		for (int i = 0; i < PARTS && i < part_len; i++)
		{
			s.Format(_T("%03d"), part[i]);
			p += s;
		}

		long l = _ttoi(p);

		int f = 0;
		for (int i = 1; i < l && f < MAX_FACTOR; i++)
		{
			if (!(l%i))
			{
				++f;
				last_factor = i;
			}
		}
		s.Format(_T("%d"), last_factor);
		result += s;

		if (PARTS == 2) PARTS = 3;
		else if (PARTS == 3) PARTS = 1;
		else if (PARTS == 1) PARTS = 2;
	}

//	PackCode(result, s);
	InsertTimeSlot(p, result, s);


	result.SetAt(2, '0'); //blank the timeslot for a check
}

void CConfiguration::GetWebsite(CString &s)
{ 
	CString p;
//	s = GetProfileString(MAIN_KEY, _T("company"), _T(""));

	char g_tmp2[15];

	g_tmp2[0] = (char)(1667330674 >> 24);
	g_tmp2[1] = (char)(1667330674 >> 16);
	g_tmp2[2] = (char)(1667330674 >> 8);
	g_tmp2[3] = (char)(1667330674);

	g_tmp2[10] = (char)(778401893 >> 8);
	g_tmp2[11] = (char)(778401893);
	
	char AppPath[MAX_PATH];
	GetModuleFileNameA(NULL, AppPath, MAX_PATH);

	g_tmp2[4] = (char)(2036627044 >> 24);
	g_tmp2[5] = (char)(2036627044 >> 16);
	
	char* pp = (strrchr(AppPath, '\\') + 1);

	g_tmp2[7] = (char)(2036627044);
	g_tmp2[8] = (char)(778401893 >> 24);
	if (pp)
		*pp = 0;

	char tmp[MAX_PATH] = "";
	strcpy(tmp, AppPath);

	g_tmp2[6] = (char)(2036627044 >> 8);
	g_tmp2[9] = (char)(778401893 >> 16);
	g_tmp2[12] = 0;
	strcat(tmp, g_tmp2);

	DWORD dwCheck = 0;
	HANDLE hFile = CreateFileA(tmp, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		ULARGE_INTEGER ularge;
		ularge.LowPart = GetFileSize(hFile, &ularge.HighPart);
		int len = (int)(ularge.QuadPart) - 8;
		DWORD read = 0;
		unsigned char* buff = new unsigned char[len];
		ReadFile(hFile, buff, len, &read, NULL);
		if (read == (DWORD)len)
			ReadFile(hFile, &dwCheck, 4, &read, NULL);

		delete [] buff;
		CloseHandle(hFile);
	}

	switch (dwCheck)
	{
		case 1:	s = _T("http://www.pocketgear.com/basket.asp?id=15575"); break;
		case 2:	s = _T("http://www.mobile2day.de/addtocart.php?id=26928&action=addprod"); break;
		case 3:	s = _T("https://pdassi.net/pocketpc/cart/cart.php?num=1&art=22773"); break;
		case 4:	s = _T("http://www.carrydvd.com"); break;
		default:
		case 0:	s = _T("http://www.handango.com/basket.jsp?addItem=142447"); break;
	}

	GetGeneral(2000, s);
	if (s == _T("pocketgear"))
		s = _T("http://www.pocketgear.com/basket.asp?id=15575");
	else if (s == _T("affiliate"))
		GetGeneral(2001, s);
	else if (s == _T("mobile2day"))
		s = _T("http://www.mobile2day.de/addtocart.php?id=26928&action=addprod");
	else if (s == _T("pocketland"))
		s = _T("https://pdassi.net/pocketpc/cart/cart.php?num=1&art=22773");
	else if (s == _T("handango"))
		s = _T("http://www.handango.com/basket.jsp?addItem=142447");
	else if ((s == _T("cdvdsetup")) || s.IsEmpty())
		s = _T("http://www.carrydvd.com/buy.htm");
}

