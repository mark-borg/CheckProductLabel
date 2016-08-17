// checklabel.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "checklabel.h"
#include "checklabelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChecklabelApp

BEGIN_MESSAGE_MAP(CChecklabelApp, CWinApp)
	//{{AFX_MSG_MAP(CChecklabelApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChecklabelApp construction

CChecklabelApp::CChecklabelApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CChecklabelApp object

CChecklabelApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CChecklabelApp initialization

BOOL CChecklabelApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	
	ParseCommandLine(cmd_line_params);

#ifdef SAVE_TRACE_LOG
	trace_file = new ofstream( "d://trace.txt" );
	if ( ! trace_file )
		AfxMessageBox( "Cannot open trace log. Debug information not saved!" );
	else
		trace_log = *trace_file;
#endif


	CChecklabelDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}




CChecklabelApp::~CChecklabelApp()
{
#ifdef SAVE_TRACE_LOG
	if ( trace_file )
	{	
		trace_file->close();
		delete trace_file;
	}
#endif
}

