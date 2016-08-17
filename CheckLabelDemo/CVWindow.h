#pragma once

// CVWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCVWindow window

class CCVWindow : public CStatic
{
// Construction
public:
	CCVWindow();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCVWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCVWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCVWindow)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
