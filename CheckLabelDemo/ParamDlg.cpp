// ParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "checklabel.h"
#include "ParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CParamCtrlEx, CParamCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CParamCtrlEx::CParamCtrlEx(DWORD style) : CParamCtrl(style), mCount(0)
{
	mCurrentRect.SetRect(0, 0, 0, 0);
	mFont.CreateFont(20,0,0,0,FW_SEMIBOLD,0,0,0,0,0,0,0,0,"Arial");
}

CString CParamCtrlEx::PushValue(BYTE type)
{ 
	CString s;
	if (mCurrentRect.IsRectEmpty())
		return s;
	mCurrentRect.type = type;
	m_coordinates.Add(mCurrentRect);
	++mCount;
	s.Format("%d - %d %d %d %d", mCount, mCurrentRect.left, 
									mCurrentRect.top, mCurrentRect.bottom, 
									mCurrentRect.right);
	return s;
}

void CParamCtrlEx::SetCurrentRect(CPoint &pt1, CPoint &pt2)
{
	CRectEx r;
	GetWindowRect(&r);
	GetParent()->ScreenToClient(&r);
	pt1-=r.TopLeft();
	pt2-=r.TopLeft();
	mCurrentRect.SetRect(pt1, pt2);
	mCurrentRect.OffsetRect(-2,-2); // shift by control frame width
	mCurrentRect.NormalizeRect();
}

void CParamCtrlEx::RemoveValue(int i)
{
	if (i < 0 || i > m_coordinates.GetUpperBound())
		return;
	m_coordinates.RemoveAt(i);
	mCount=m_coordinates.GetSize();
}

void CParamCtrlEx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (mImg)
	{
		cvPaintImage(GetSafeHwnd(), dc.GetSafeHdc());
	}
	
	CBrush rbrush(RGB(255, 0, 0));
	CBrush gbrush(RGB(0, 255, 0));
	CBrush bbrush(RGB(0, 0, 255));

	int count = m_coordinates.GetSize();
	CString s;
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(&mFont);
	dc.SetTextColor(RGB(0, 255, 0));
	for (int i = 0; i < count; i++)
	{
		s.Format("%d", i+1);
		CRectEx r = m_coordinates.GetAt(i);
		CBrush* pBrush = &rbrush;

		if (r.type == REX_LABEL_REGION)
			pBrush = &gbrush;
		else if (r.type == REX_SEARCH_REGION)
			pBrush = &bbrush;
// we should be calculating this precisely!
		r.OffsetRect(0, 3);
		dc.FrameRect(&r, pBrush);
		
		CRect num_rect(r.left, r.top, r.left+20, r.top+20);
		dc.DrawText(s, s.GetLength(), &num_rect, DT_EDITCONTROL|DT_LEFT|DT_VCENTER);
	}

	if (!mCurrentRect.IsRectEmpty())
	{
		dc.FrameRect(&mCurrentRect, &rbrush);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CParamDlg dialog


CParamDlg::CParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamDlg::IDD, pParent), mButtonDown(0), mStartPt(0, 0),
		m_master(PC_RESIZE_TO_IMAGE), m_red(RGB(255, 0, 0)), m_green(RGB(0, 255, 0)), 
		m_blue(RGB(0, 0, 255))
{
	//{{AFX_DATA_INIT(CParamDlg)
	m_Type = 2;
	//}}AFX_DATA_INIT
}


void CParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParamDlg)
	DDX_Control(pDX, IDC_BUTTON1, m_helpBtn);
	DDX_Control(pDX, IDOK, m_ok_btn);
	DDX_Control(pDX, IDCANCEL, m_cancel_btn);
	DDX_Control(pDX, IDC_TYPE_FRAME, m_typeCtrl);
	DDX_Control(pDX, IDC_SEARCH_RGN, m_search_ctrl);
	DDX_Control(pDX, IDC_REMOVE, m_removeCtrl);
	DDX_Control(pDX, IDC_ADD, m_addCtrl);
	DDX_Control(pDX, IDC_LABEL_RGN, m_label_ctrl);
	DDX_Control(pDX, IDC_SEARCH_MSR_RGN, m_search_msr_ctrl);
	DDX_Control(pDX, IDC_RED, m_red);
	DDX_Control(pDX, IDC_GREEN, m_green);
	DDX_Control(pDX, IDC_BLUE, m_blue);
	DDX_Control(pDX, IDC_ITEMS, m_list);
	DDX_Control(pDX, IDC_MASTER, m_master);
	DDX_Radio(pDX, IDC_SEARCH_RGN, m_Type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParamDlg, CDialog)
	//{{AFX_MSG_MAP(CParamDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_LBN_SELCHANGE(IDC_ITEMS, OnSelchangeItems)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParamDlg message handlers

BOOL CParamDlg::OnInitDialog() 
{
	m_master.SetStyle(PC_RESIZE_TO_IMAGE);
	m_master.SetName("Master");

	CDialog::OnInitDialog();

	CRect crect;
	m_master.GetClientRect(&crect);

	SIZE size;

	m_master.ResizeWindow(&size);

	size.cx -= crect.Width();
	size.cy -= crect.Height();

	if ( size.cx > 1024 || size.cy > 1024 )		// TEMP FIX!!! takes care of when no video source is found
	{
		size.cx = size.cy = 300;
	}
	
	MoveControlsDown(size.cy+GetSystemMetrics(SM_CYSMCAPTION));
	MoveControlsRight(size.cx+GetSystemMetrics(SM_CYSMCAPTION));

	GetWindowRect(&crect);

	MoveWindow(crect.left, crect.top, crect.Width()+size.cx+GetSystemMetrics(SM_CYSMCAPTION), 
			   crect.Height()+size.cy+GetSystemMetrics(SM_CYSMCAPTION));

	CString s;
	m_list.ResetContent();
	int items = m_master.GetNumberOfItems();
	for (int i = 0; i < items; i++)
	{
		CRectEx r = m_master.GetRectAt(i);
		s.Format("%d - %d %d %d %d", i+1, r.left, r.top, r.bottom, r.right);
		m_list.AddString(s);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CParamDlg::OnAdd() 
{
	CString s;
	UpdateData();
	
	BYTE type;
	
	switch (m_Type)
	{
		case 0: type = REX_SEARCH_SIZE_REGION; break;
		case 1: type = REX_LABEL_REGION;       break;
		case 2: type = REX_SEARCH_REGION;      break;
		default: AfxMessageBox("Please select a type"); return;
	}
	
	s = m_master.PushValue(type);
	if (!s.IsEmpty())
		m_list.AddString(s);
	m_master.ZeroCurrentRect();
	m_master.Invalidate();
}

void CParamDlg::OnRemove() 
{
	CString s;
	int idx = m_list.GetCurSel();
	if (idx < 0)
		return;
	m_master.RemoveValue(idx);

	m_list.ResetContent();
	int items = m_master.GetNumberOfItems();
	for (int i = 0; i < items; i++)
	{
		CRectEx r = m_master.GetRectAt(i);
		s.Format("%d - %d %d %d %d", i+1, r.left, r.top, r.bottom, r.right);
		m_list.AddString(s);
	}
	
	Invalidate();
}

void CParamDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	mButtonDown = false;
	m_master.SetCurrentRect(mStartPt, point);	
	Invalidate();
	CDialog::OnLButtonUp(nFlags, point);
}

void CParamDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	mButtonDown = true;
	mStartPt = point;
	CDialog::OnLButtonDown(nFlags, point);
}


void CParamDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (mButtonDown)
	{
		CBrush brush(RGB(255, 0, 0));
		CDC *dc = GetDC(); // device context for painting
		CRectEx r, rlast, p;
		rlast.SetRect(mStartPt, mPrevPoint);
		rlast.NormalizeRect();
		
		r.SetRect(mStartPt, point);
		r.NormalizeRect();
		dc->FrameRect(&r, &brush);

		p.SubtractRect(rlast, r);
		r.DeflateRect(1,1);

		InvalidateRect(r);
		InvalidateRect(p);
		ReleaseDC(dc);
		mPrevPoint = point;
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CParamDlg::OnOK() 
{
	CDialog::OnOK();
}

void CParamDlg::OnSelchangeItems() 
{
	int idx = m_list.GetCurSel();
	if (idx < 0)
		return;
	CRectEx r = m_master.GetRectAt(idx);
	m_Type = r.type;
	UpdateData(FALSE);
}

void CParamDlg::PushValue(CRectEx& r)
{
	m_master.SetCurrentRect(r);
	m_master.PushValue(r.type);
	m_master.ZeroCurrentRect();
}

/////////////////////////////////////////////////////////////////////////////
// CColouredCtrl

CColouredCtrl::CColouredCtrl()
{
}

CColouredCtrl::CColouredCtrl(COLORREF col)
{ SetColour(col);
}

CColouredCtrl::~CColouredCtrl()
{
}


BEGIN_MESSAGE_MAP(CColouredCtrl, CStatic)
	//{{AFX_MSG_MAP(CColouredCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColouredCtrl message handlers

void CColouredCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rect = dc.m_ps.rcPaint;
	dc.FillSolidRect(rect, mColor);
}

void CParamDlg::MoveControlsDown(int size)
{
	CRect r;
	m_ok_btn.GetWindowRect(&r);
	ScreenToClient(&r);
	m_ok_btn.MoveWindow(r.left, r.top+size, r.Width(), r.Height());
	
	m_cancel_btn.GetWindowRect(&r);
	ScreenToClient(&r);
	m_cancel_btn.MoveWindow(r.left, r.top+size, r.Width(), r.Height());

	m_helpBtn.GetWindowRect(&r);
	ScreenToClient(&r);
	m_helpBtn.MoveWindow(r.left, r.top+size, r.Width(), r.Height());
}

void CParamDlg::MoveControlsRight(int size)
{
	CRect r;
	m_typeCtrl.GetWindowRect(&r);
	ScreenToClient(&r);
	m_typeCtrl.MoveWindow(r.left+size, r.top, r.Width(), r.Height());	

	m_search_ctrl.GetWindowRect(&r);
	ScreenToClient(&r);
	m_search_ctrl.MoveWindow(r.left+size, r.top, r.Width(), r.Height());	
	
	m_removeCtrl.GetWindowRect(&r);
	ScreenToClient(&r);
	m_removeCtrl.MoveWindow(r.left+size, r.top, r.Width(), r.Height());	
	
	m_addCtrl.GetWindowRect(&r);
	ScreenToClient(&r);
	m_addCtrl.MoveWindow(r.left+size, r.top, r.Width(), r.Height());	
	
	m_red.GetWindowRect(&r);
	ScreenToClient(&r);
	m_red.MoveWindow(r.left+size, r.top, r.Width(), r.Height());	
	
	m_green.GetWindowRect(&r);
	ScreenToClient(&r);
	m_green.MoveWindow(r.left+size, r.top, r.Width(), r.Height());	
	
	m_blue.GetWindowRect(&r);
	ScreenToClient(&r);
	m_blue.MoveWindow(r.left+size, r.top, r.Width(), r.Height());	
	
	m_list.GetWindowRect(&r);
	ScreenToClient(&r);
	m_list.MoveWindow(r.left+size, r.top, r.Width(), r.Height());	

	m_label_ctrl.GetWindowRect(&r);
	ScreenToClient(&r);
	m_label_ctrl.MoveWindow(r.left+size, r.top, r.Width(), r.Height());	

	m_search_msr_ctrl.GetWindowRect(&r);
	ScreenToClient(&r);
	m_search_msr_ctrl.MoveWindow(r.left+size, r.top, r.Width(), r.Height());	
}

void CParamDlg::OnButton1() 
{
	DisplayHelp("params.html");	
}
