#if !defined(AFX_ACKNOWLEDGEMENTS_H__671985FB_0761_4C39_8EF7_F41428F638D0__INCLUDED_)
#define AFX_ACKNOWLEDGEMENTS_H__671985FB_0761_4C39_8EF7_F41428F638D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Acknowledgements.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAcknowledgements dialog

class CAcknowledgements : public CDialog
{
// Construction
public:
	CAcknowledgements(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAcknowledgements)
	enum { IDD = IDD_ACKNOWLEDGEMENTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcknowledgements)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAcknowledgements)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACKNOWLEDGEMENTS_H__671985FB_0761_4C39_8EF7_F41428F638D0__INCLUDED_)
