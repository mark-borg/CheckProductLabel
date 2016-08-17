#pragma once

// ParamDlg.h : header file
//
#include "gui\ParamCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CColouredCtrl window

class CColouredCtrl : public CStatic
{
// Construction
public:
	CColouredCtrl();
	CColouredCtrl(COLORREF col);

// Attributes
public:

// Operations
public:
	void SetColour(COLORREF col)
		{ mColor=col; }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColouredCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColouredCtrl();

	// Generated message map functions
protected:
	COLORREF mColor;
	//{{AFX_MSG(CColouredCtrl)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CParamDlg dialog

#define REX_SEARCH_SIZE_REGION 0x00
#define REX_LABEL_REGION       0x01
#define REX_SEARCH_REGION      0x02

class CRectEx : public CRect
{
public:
	CRectEx() {}
	CRectEx(int l, int t, int r, int b, int type) : CRect(l, t, r, b) { this->type = type;}
	

	BYTE type;

	CRectEx(const CRectEx& srcRect)
		{ ::CopyRect(this, &srcRect); type = srcRect.type; }
	void operator = (const CRectEx& srcRect)
		{ ::CopyRect(this, &srcRect); type = srcRect.type; }
};

class CParamCtrlEx : public CParamCtrl
{
// Construction
public:
	CParamCtrlEx(DWORD style=(PC_STRETCH_IMAGE|PC_NO_CAMERA_CONNECT));

	void SetCurrentRect(CPoint &pt1, CPoint &pt2);

	CString PushValue(BYTE type);
	void RemoveValue(int i);

	void ZeroCurrentRect()
		{ mCurrentRect.SetRect(0, 0, 0, 0);
		}
	
	int GetNumberOfItems()
		{ return m_coordinates.GetSize(); }
	CRectEx& GetRectAt(int i)
		{ return m_coordinates[i]; }
	CRectEx& GetCurrentRect()
		{ return mCurrentRect; }
	void SetCurrentRect(CRectEx& r)
		{ mCurrentRect= r; }
protected:
	CFont mFont;
	CArray <CRectEx, CRectEx& > m_coordinates;

	CRectEx mCurrentRect;
	int mCount;

	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

class CParamDlg : public CDialog
{
// Construction
public:
	void MoveControlsDown(int size);
	void MoveControlsRight(int size);
	CParamDlg(CWnd* pParent = NULL);   // standard constructor

	void PushValue(CRectEx& r);

	CRectEx& GetRectAt(int i)
		{ return m_master.GetRectAt(i); }

	int GetNumberOfItems()
		{ return m_master.GetNumberOfItems(); }

	void SetPath(const char* video, const char* train)
		{ m_master.SetPath(video, train);
		}
// Dialog Data
	//{{AFX_DATA(CParamDlg)
	enum { IDD = IDD_PARAMS };
	CButton	m_helpBtn;
	CButton	m_ok_btn;
	CButton	m_cancel_btn;
	CButton	m_typeCtrl;
	CButton	m_search_ctrl;
	CButton	m_removeCtrl;
	CButton	m_addCtrl;
	CButton	m_label_ctrl;
	CButton	m_search_msr_ctrl;
	CColouredCtrl	m_red;
	CColouredCtrl	m_green;
	CColouredCtrl	m_blue;
	CListBox	m_list;
	CParamCtrlEx	m_master;
	int		m_Type;
	//}}AFX_DATA
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPoint mStartPt, mPrevPoint;
	bool mButtonDown;
	// Generated message map functions
	//{{AFX_MSG(CParamDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnOK();
	afx_msg void OnSelchangeItems();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
