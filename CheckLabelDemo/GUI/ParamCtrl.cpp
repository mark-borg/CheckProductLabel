// ParamCtrl.cpp : implementation file
//

#include "..\stdafx.h"
#include "ParamCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

DWORD CParamCtrl::mStyle=0;
char CParamCtrl::mfilename[MAX_PATH];

/////////////////////////////////////////////////////////////////////////////
// CParamCtrl

CParamCtrl::CParamCtrl(DWORD style) : mImg(0), mpCamera(0)
{
	mStyle = style;
}

CParamCtrl::~CParamCtrl()
{
}


BEGIN_MESSAGE_MAP(CParamCtrl, CStatic)
	//{{AFX_MSG_MAP(CParamCtrl)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamCtrl message handlers

void CParamCtrl::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();

	cvNamedWindowEx( mName, GetSafeHwnd(), mStyle );

	if (!(mStyle & PC_NO_CAMERA_CONNECT))
		mpCamera = new NumberedFileCamera( (const char*)mVideoPath, (const char*)mTrainingPath );
	
	if (mpCamera)
	{
		mpCamera->set_output_greyscale();
		if ( ! mpCamera->open_source() )
		{
			AfxMessageBox("Error starting camera");
			return;
		}
		if (mImg)
			cvReleaseImage( &mImg );

		// get an initial image - will be used as template
		mImg = mpCamera->acquire_image();
		if (!mImg)
		{
			AfxMessageBox("Error acquiring image");
			return;
		}
		cvShowImageEx(GetSafeHwnd(), mImg);
	}
}

void CParamCtrl::ResizeWindow(SIZE* size)
{
	CRect rect, prect;
	GetParent()->GetWindowRect(&prect);
	GetWindowRect(&rect);
	cvMoveWindowEx(GetSafeHwnd(), rect.left-prect.left, 
		rect.top-prect.top-GetSystemMetrics(SM_CYSMCAPTION), 
		0, 0, size);
}

void CParamCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (mImg)
	{
		cvPaintImage(GetSafeHwnd(), dc.GetSafeHdc());
	}
}

void CParamCtrl::OnDestroy() 
{
	if (mpCamera)
	{
		if (mImg)
			cvReleaseImage( &mImg );
		mpCamera->close_source();
		delete mpCamera;
		mpCamera = 0;
	}
	CStatic::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
// CStatusLabel

CStatusLabel::CStatusLabel()
{
	mCurrentStatus=STATUS_NONE;
	mFont.CreateFont(25, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Arial");
}

CStatusLabel::~CStatusLabel()
{
}


BEGIN_MESSAGE_MAP(CStatusLabel, CStatic)
	//{{AFX_MSG_MAP(CStatusLabel)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusLabel message handlers

void CStatusLabel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (mCurrentStatus==STATUS_OK)
	{
		CRect rect;
		GetClientRect(&rect);
		dc.FillSolidRect(&rect, RGB(0, 255, 0));

		dc.SelectObject(mFont);
		dc.DrawText("OK", 2, rect, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	else if (mCurrentStatus==STATUS_FAILED)
	{
		CRect rect;
		GetClientRect(&rect);
		dc.FillSolidRect(&rect, RGB(255, 0, 0));
		dc.SelectObject(mFont);
		dc.DrawText("REJECTED", strlen("REJECTED"), rect, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	else
	{
		// do nothing
	}

	// Do not call CStatic::OnPaint() for painting messages
}

void CStatusLabel::SetStatus(CurrentStatus status)
{
	mCurrentStatus=status;
	Invalidate();
}
