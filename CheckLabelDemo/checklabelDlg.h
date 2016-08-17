// checklabelDlg.h : header file
//

#pragma once

#include "gui\ParamCtrl.h"
#include "gui\MessageWindow.h"

using namespace std;
#define NUMBER_OF_SAMPLES 3
/////////////////////////////////////////////////////////////////////////////
// CChecklabelDlg dialog

class CChecklabelDlg : public CDialog
{
// Construction
public:
	void EnableWin(int seq);
	CChecklabelDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CChecklabelDlg)
	enum { IDD = IDD_CHECKLABEL_DIALOG };
	CStatic	m_perc_3;
	CStatic	m_perc_2;
	CStatic	m_perc_1;
	CEdit	m_count_3;
	CEdit	m_count_2;
	CEdit	m_count_1;
	CStatic	m_state_3;
	CStatic	m_state_2;
	CStatic	m_state_1;
	CEdit	m_horz_dispCtrl;
	CButton	m_debugCtrl;
	CButton	m_sample_1ctrl;
	CButton	m_sample_2ctrl;
	CButton	m_sample_3ctrl;
	CEdit	m_vert_dispCtrl;
	CEdit	m_variationCtrl;
	CEdit	m_rot_tolCtrl;
	CButton	m_ParamsCtrl;
	CButton	m_trainingCtrl;
	CStatusLabel	m_Status;
	CButton	m_stepThroughCtrl;
	CButton	m_stepCtrl;
	CButton	m_startCtrl;
	CMessageWindow	m_messages;
	CParamCtrl	m_ResultCtrl;
	CParamCtrl	m_InputCtrl;
	int		m_sample;
	BOOL	m_debug;
	int		m_horiz_displ;
	int		m_rotation_tol;
	int		m_vertical_disp;
	float	m_variation;
	BOOL	m_StepThrough;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChecklabelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	void LoadSample(int sample);

// Implementation
protected:
	HICON m_hIcon;
	bool mbSetToStart[NUMBER_OF_SAMPLES];
	// (1) train the algorithm to find the bottle outline 
	IplImage* mImg, *mEdge_map;
	NumberedFileCamera *mpCamera, *mpCamera2;
	CString mVideoPath, mTrainingPath, mTestPath;
	
	// (1) train the algorithm to find the bottle outline 
	ContainerOutlineFinder *mBottle;
	ContainerAreaFinder    *mLabel;
	AreaVarianceDetector   *mVar;
	CMenu mMenu;

	enum { MAX_SAMPLES = 3 };
	CString VideoPath[MAX_SAMPLES], TrainFileMask[MAX_SAMPLES], TestFileMask[MAX_SAMPLES];
	int TemplateSize[MAX_SAMPLES], TemplateDistance[MAX_SAMPLES];
	int EdgeLoThreshold[MAX_SAMPLES], EdgeHiThreshold[MAX_SAMPLES];

	vector< CvRect > mRoi;
	CvRect label_roi;
	CvRect search_roi;
	bool DoTraining();
	bool DoStart();
	bool mStep, mTrainWasOK;
	int tmpl_size;
	int tmpl_dist;
	bool m_bStop;
	CFont mFont;
	void SetCounters(BYTE which, float amount, float perc);
	// Generated message map functions
	//{{AFX_MSG(CChecklabelDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnTraining();
	afx_msg void OnStart();
	virtual void OnOK();
	afx_msg void OnStepThrough();
	afx_msg void OnStep();
	virtual void OnCancel();
	afx_msg void OnSample1();
	afx_msg void OnSample2();
	afx_msg void OnSample3();
	afx_msg void OnHelp();
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	afx_msg LRESULT OnCheckDevices(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
