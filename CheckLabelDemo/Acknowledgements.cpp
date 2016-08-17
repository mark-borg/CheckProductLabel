// Acknowledgements.cpp : implementation file
//

#include "stdafx.h"
#include "checklabel.h"
#include "Acknowledgements.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAcknowledgements dialog


CAcknowledgements::CAcknowledgements(CWnd* pParent /*=NULL*/)
	: CDialog(CAcknowledgements::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAcknowledgements)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAcknowledgements::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAcknowledgements)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAcknowledgements, CDialog)
	//{{AFX_MSG_MAP(CAcknowledgements)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcknowledgements message handlers
