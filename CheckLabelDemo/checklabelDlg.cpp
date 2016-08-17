// checklabelDlg.cpp : implementation file
//

#include "stdafx.h"
#include "checkLabel.h"
#include "checklabelDlg.h"
#include "ParamDlg.h"
#include "Acknowledgements.h"
#include <fstream>

using namespace std;
#define WM_THREADTOWINDOW WM_USER + 10
#define TEST_1



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BLOCK_1 1
#define BLOCK_2 2
#define BLOCK_3 4
#define BLOCK_4 8
#define ALL_BLOCKS 7

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnAcknowledgements();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_ACKNOWLEDGEMENTS, OnAcknowledgements)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChecklabelDlg dialog

CChecklabelDlg::CChecklabelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChecklabelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChecklabelDlg)
	m_sample = 0;
	m_debug = FALSE;
	m_horiz_displ = 10;
	m_rotation_tol = 3;
	m_vertical_disp = 5;
	m_variation = (float)0.05;
	m_StepThrough = FALSE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mpCamera  = 0;
	mpCamera2 = 0;

	mBottle = 0;
	mLabel = 0;
	mVar = 0;
	mbSetToStart[0]=false;
	mbSetToStart[1]=false;
	mbSetToStart[2]=false;
}

void CChecklabelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChecklabelDlg)
	DDX_Control(pDX, IDC_PERC_3, m_perc_3);
	DDX_Control(pDX, IDC_PERC_2, m_perc_2);
	DDX_Control(pDX, IDC_PERC_1, m_perc_1);
	DDX_Control(pDX, IDC_COUNT_3, m_count_3);
	DDX_Control(pDX, IDC_COUNT_2, m_count_2);
	DDX_Control(pDX, IDC_COUNT_1, m_count_1);
	DDX_Control(pDX, IDC_STATE_3, m_state_3);
	DDX_Control(pDX, IDC_STATE_2, m_state_2);
	DDX_Control(pDX, IDC_STATE_1, m_state_1);
	DDX_Control(pDX, IDC_HORZ_DISP, m_horz_dispCtrl);
	DDX_Control(pDX, IDC_DEBUG, m_debugCtrl);
	DDX_Control(pDX, IDC_SAMPLE_1, m_sample_1ctrl);
	DDX_Control(pDX, IDC_SAMPLE_2, m_sample_2ctrl);
	DDX_Control(pDX, IDC_SAMPLE_3, m_sample_3ctrl);
	DDX_Control(pDX, IDC_VERT_DISP, m_vert_dispCtrl);
	DDX_Control(pDX, IDC_VARIATIO, m_variationCtrl);
	DDX_Control(pDX, IDC_ROT_TOL, m_rot_tolCtrl);
	DDX_Control(pDX, IDOK, m_ParamsCtrl);
	DDX_Control(pDX, IDC_TRAINING, m_trainingCtrl);
	DDX_Control(pDX, IDC_STATUS, m_Status);
	DDX_Control(pDX, IDC_STEP_THROUGH, m_stepThroughCtrl);
	DDX_Control(pDX, IDC_STEP, m_stepCtrl);
	DDX_Control(pDX, IDC_START, m_startCtrl);
	DDX_Control(pDX, IDC_MESSAGES, m_messages);
	DDX_Control(pDX, IDC_RESULT, m_ResultCtrl);
	DDX_Control(pDX, IDC_INPUT, m_InputCtrl);
	DDX_Radio(pDX, IDC_SAMPLE_1, m_sample);
	DDX_Check(pDX, IDC_DEBUG, m_debug);
	DDX_Text(pDX, IDC_HORZ_DISP, m_horiz_displ);
	DDX_Text(pDX, IDC_ROT_TOL, m_rotation_tol);
	DDX_Text(pDX, IDC_VERT_DISP, m_vertical_disp);
	DDX_Text(pDX, IDC_VARIATIO, m_variation);
	DDX_Check(pDX, IDC_STEP_THROUGH, m_StepThrough);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChecklabelDlg, CDialog)
	//{{AFX_MSG_MAP(CChecklabelDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_TRAINING, OnTraining)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_STEP_THROUGH, OnStepThrough)
	ON_BN_CLICKED(IDC_STEP, OnStep)
	ON_BN_CLICKED(IDC_SAMPLE_1, OnSample1)
	ON_BN_CLICKED(IDC_SAMPLE_2, OnSample2)
	ON_BN_CLICKED(IDC_SAMPLE_3, OnSample3)
	ON_COMMAND(IDR_HELP, OnHelp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_THREADTOWINDOW, OnCheckDevices)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChecklabelDlg message handlers

BOOL CChecklabelDlg::OnInitDialog()
{
	m_ResultCtrl.SetStyle(PC_NO_CAMERA_CONNECT|PC_STRETCH_IMAGE);
	m_InputCtrl.SetStyle(PC_NO_CAMERA_CONNECT|PC_STRETCH_IMAGE);

	m_ResultCtrl.SetName("Result");
	m_InputCtrl.SetName("Input");

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	mMenu.LoadMenu(IDR_MENU1);
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// set defaults
	const int samples = MAX_SAMPLES;
	const int max_items=20;
	int rSize[samples];
	
	CRectEx r[samples][max_items];


	// use command-line path (if any) as the root path; else use local path
	CString root_path = "";
	CChecklabelApp* app = (CChecklabelApp*) AfxGetApp();
	if ( app )
	{
		if ( !app->cmd_line_params.m_strFileName.IsEmpty() )
		{
			root_path = app->cmd_line_params.m_strFileName;
			if ( !root_path.IsEmpty() )
				root_path += "\\";
		}
	}


	// search regions used to measure bottle position
	for ( int sn = 0; sn < MAX_SAMPLES; ++sn )
	{
		char path[200];
		sprintf( path, "%ssample%d.cfg", root_path, sn+1 );
		ifstream cfg( path );

		if ( ! cfg || cfg.eof() )
		{
			char msg[200];
			sprintf( msg, "No configuration file 'sample%d.cfg' found for sample %d!\nPlease ensure that this file is in the same folder as checklabel.exe\nPlease re-install the program if the files are missing.", sn+1, sn+1 );
			AfxMessageBox( msg, MB_ICONERROR );
			rSize[sn]=0;
		}
		else
		{
			char tmp[MAX_PATH];
			cfg >> tmp; VideoPath[sn] = root_path + tmp;
			cfg >> tmp; TrainFileMask[sn] = tmp;
			cfg >> tmp; TestFileMask[sn] = tmp;
			
			cfg >> rSize[sn];

			for ( int n = 0; n < rSize[sn]; ++n )
			{
				int x1, y1, x2, y2, typ;
				cfg >> x1;
				cfg >> y1;
				cfg >> x2;
				cfg >> y2;
				cfg >> typ;
				r[sn][n]=CRectEx( x1, y1, x2, y2, typ );
			}

			cfg >> TemplateSize[sn];
			cfg >> TemplateDistance[sn];

			cfg >> EdgeLoThreshold[sn];
			cfg >> EdgeHiThreshold[sn];
		}
		cfg.close();
	}


	
	CString s;
	for (int k=0; k<samples; k++)
	{
		CString sSample1, sSample2;
		sSample1.Format("sample%d\\Items", k);
		sSample2 = "sample%d\\Items%d";
		
		int items = rSize[k];
		gConfiguration.WriteProfileInt(sSample1, "Total", items);

		for (int i=0; i<items; i++)
		{
			s.Format(sSample2, k, i);

			gConfiguration.WriteProfileInt(s, "left",   r[k][i].left);
			gConfiguration.WriteProfileInt(s, "right",  r[k][i].right);
			gConfiguration.WriteProfileInt(s, "top",    r[k][i].top);
			gConfiguration.WriteProfileInt(s, "bottom", r[k][i].bottom);
			gConfiguration.WriteProfileInt(s, "type",   r[k][i].type);
		}
	}
	OnStepThrough();
	mFont.CreateFont(15, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, "Arial");
	EnableWin(0);
	
	m_state_1.SetFont(&mFont);
	m_state_2.SetFont(&mFont);
	m_state_3.SetFont(&mFont);

	m_state_1.SetWindowText("");
	m_state_2.SetWindowText("");
	m_state_3.SetWindowText("");

	SetCounters(ALL_BLOCKS, 0, 0);

	PostMessage(WM_THREADTOWINDOW);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChecklabelDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChecklabelDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChecklabelDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CChecklabelDlg::OnDestroy() 
{
	if (mpCamera)
		delete mpCamera;
	
	if (mpCamera2)
		delete mpCamera2;

	if (mBottle)
		delete mBottle;

	if (mLabel)
		delete mLabel;

	if (mVar)
		delete mVar;

	CDialog::OnDestroy();
}

void CChecklabelDlg::OnTraining() 
{
	EnableWin(0);
	m_bStop = false;
	mTrainWasOK=false;
	mStep=true;
	if (!DoTraining())
		m_messages << "Error during training" << ENDL;
	else
		mTrainWasOK=true;
	EnableWin(BLOCK_3|BLOCK_1);
}

bool CChecklabelDlg::DoTraining() 
{
	UpdateData();

	if (m_sample >= 0 && m_sample < MAX_SAMPLES)
	{
		tmpl_size = TemplateSize[m_sample];
		tmpl_dist = TemplateDistance[m_sample];
		mVideoPath=VideoPath[m_sample];
		mTrainingPath=TrainFileMask[m_sample];
		mTestPath=TestFileMask[m_sample];
	}
	else
	{
		AfxMessageBox("Please select a sample!");
		return false;
	}

	LoadSample(m_sample);
	
	//=================== TRAINING ==========================================
	m_messages << "===== Starting training proces...." << ENDL;

	if (mBottle)
		delete mBottle;

	mBottle = new ContainerOutlineFinder();

	if (mLabel)
		delete mLabel;

	mLabel = new ContainerAreaFinder();

	if (mVar)
		delete mVar;

	mVar = new AreaVarianceDetector();

	if (mpCamera)
		delete mpCamera;
	
	mpCamera = new NumberedFileCamera( (const char*)mVideoPath, (const char*)mTrainingPath );
	
	mpCamera->set_output_greyscale();
	if ( ! mpCamera->open_source() )
		return false;

	// get an initial image - will be used as template
	mImg = mpCamera->acquire_image();
	if ( ! mImg )
		return false;
	
	m_messages << "Training started.... please wait; this might take several minutes..." << ENDL;

	if (m_debug)
		mBottle->set_debug();

	// find edge contours
	mEdge_map = cvCreateImage( cvGetSize( mImg ), IPL_DEPTH_8U, 1 );
	assert( mEdge_map );
	cvCanny( mImg, mEdge_map, EdgeLoThreshold[m_sample], EdgeHiThreshold[m_sample] );

	// the user adds the regions containing linear parts of the outline of the bottle
	for ( int n = 0; n < mRoi.size(); ++n )
		if ( ! mBottle->add_model_region( mEdge_map, mRoi[ n ] ) )
			m_messages << "This region will not be used." << ENDL;


	// finalise the bottle outline model
	if ( ! mBottle->finalise_model() )
		return false;

	// (2) training the program to find the label area

	// find regions of strong texture inside the label that can be used to locate the label
	HighTexturedAreaFinder texture;
	
	if (m_debug)
		texture.set_debug();
	texture.set_search_area( label_roi );
	texture.set_min_template_distance( tmpl_dist );
	texture.set_template_width( tmpl_size );
	texture.set_template_height( tmpl_size );
	const int max_search_areas = 4;

	bool ok = texture.process_image( mImg );
	if ( ! ok )
	{
		m_messages << "Error initialising texture search!" << ENDL;
		return false;
	}


	if (m_debug)
		mLabel->set_debug();
	mLabel->set_label_area( label_roi );
	mLabel->set_search_area( search_roi );

	// add the areas of strong texture found above to the label finder object
	do
	{
		CvRect area;
		ok = false;
		if ( mLabel->num_templates() < max_search_areas && 
			 ( ok = texture.find_strong_textured_area( area ) ) )
			mLabel->add_template_area( area );
	}
	while( ok );

	if ( mLabel->num_templates() < 3 )
	{
		m_messages << "Less than 3 label search areas found!" << ENDL;
		return false;
	}


	// learn the label model
	mLabel->finalise_model( mImg );


	// (3) training the program to learn variations in the label area
	mVar->set_area( label_roi );
	if (m_debug)
		mVar->set_debug();

	mVar->learn_sample( mImg );

	
	cvReleaseImage( &mImg );


	while( ( mImg = mpCamera->acquire_image() ) != NULL )
	{
		m_InputCtrl.ShowImage( mImg );


		// find edge contours
		assert( mEdge_map );
		cvCanny( mImg, mEdge_map, EdgeLoThreshold[m_sample], EdgeHiThreshold[m_sample] );

		
		// (3a) find and correct bottle outline
		CvMat* M = cvCreateMat( 2, 3, CV_32FC1 );
		CvMat* ML = cvCreateMat( 3, 3, CV_32FC1 );
		assert( M && ML );

		IplImage* corrected_bottle_img = cvCreateImage( cvGetSize( mImg ), IPL_DEPTH_8U, 1 );
		assert( corrected_bottle_img );

		if ( ! mBottle->process_image( mEdge_map, M ) )
			return false;
		m_messages << "Bottle is rotated by:   \t" << mBottle->get_rotation_correction() << '°' << ENDL;
		m_messages << "Bottle is displaced by: \t" << mBottle->get_displacement_correction().x << ", " << - mBottle->get_displacement_correction().y << " mm" << ENDL;

		
		// rewarp the image to align bottle with master image
		cvWarpAffine( mImg, corrected_bottle_img, M, CV_INTER_CUBIC + CV_WARP_FILL_OUTLIERS );


		// (3b) find the label and measure its position relative to bottle outline
		if ( ! mLabel->process_image( corrected_bottle_img, ML ) )
			return false;
		m_messages << "Label is rotated by (Rel. to bottle):   \t" << - mLabel->get_rotation_correction() << '°' << ENDL;
		m_messages << "Label is displaced by (Rel. to bottle): \t" << - mLabel->get_displacement_correction().x << ", " << - mLabel->get_displacement_correction().y << " mm" << ENDL;


		// (3c) combine the transformation matrices together
		CvMat* MF = cvCreateMat( 3, 3, CV_32FC1 );
		M = convert_3x2_matrix_to_3x3( M );
		cvMatMul( M, ML, MF );


		IplImage* corrected_label_img = cvCreateImage( cvGetSize( mImg ), IPL_DEPTH_8U, 1 );
		assert( corrected_label_img );

		MF = convert_3x3_matrix_to_3x2( MF );
		cvWarpAffine( mImg, corrected_label_img, MF, CV_INTER_CUBIC + CV_WARP_FILL_OUTLIERS );


		// (3d) learn label variations
		mVar->learn_sample( corrected_label_img );



		// cleanup
		cvReleaseImage( &corrected_bottle_img );
		cvReleaseImage( &corrected_label_img );
		cvReleaseMat( &M );
		cvReleaseMat( &ML );
		cvReleaseMat( &MF );
		cvReleaseImage( &mImg );

		MSG msg;
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		m_messages << ENDL;
	}


	// (3e) finalise label variation model
	mVar->finalise_model();

	// cleanup
	m_messages << "===== Training proces finished." << ENDL;
	m_messages << "===== Click 'Start' to start Online Testing of product...." << ENDL;
	cvReleaseImage( &mImg );
	mpCamera->close_source();
	
	mbSetToStart[m_sample] = true;

	return true;
}

void CChecklabelDlg::OnStart() 
{
	m_bStop = false;
	EnableWin(0);
	mStep=true;
	DoStart();
	EnableWin(BLOCK_3|BLOCK_1|BLOCK_4);
}

bool CChecklabelDlg::DoStart() 
{
	int rejected = 0;
	int accepted = 0;
	int reworked = 0;
	int total	 = 0;
	SetCounters(ALL_BLOCKS, 0, 0);
	UpdateData();
	if (!mbSetToStart[m_sample])
	{
		AfxMessageBox("Please train me first!");
		return false;
	}
	//=================== ONLINE TESTING ==========================================

	if (mpCamera2)
		delete mpCamera2;

	mpCamera2 = new NumberedFileCamera( (const char*)mVideoPath, (const char*)mTestPath );
	
	mpCamera2->set_output_greyscale();
	if ( ! mpCamera2->open_source() )
		return false;

	IplImage* result = cvCreateImage( cvSize( label_roi.width, label_roi.height ), IPL_DEPTH_8U, 3 );

	while( ( mImg = mpCamera2->acquire_image() ) != NULL )
	{
		m_InputCtrl.ShowImage( mImg );
		bool item_accepted = true;

		++total;
		// find edge contours
		assert( mEdge_map );
		cvCanny( mImg, mEdge_map, EdgeLoThreshold[m_sample], EdgeHiThreshold[m_sample] );

		
		// (1) find and correct bottle outline
		CvMat* M = cvCreateMat( 2, 3, CV_32FC1 );
		CvMat* ML = cvCreateMat( 3, 3, CV_32FC1 );
		assert( M && ML );

		IplImage* corrected_bottle_img = cvCreateImage( cvGetSize( mImg ), IPL_DEPTH_8U, 1 );
		assert( corrected_bottle_img );

		if ( ! mBottle->process_image( mEdge_map, M ) )
			return false;
		m_messages << "Bottle is rotated by:   \t" << - mBottle->get_rotation_correction() << '°' << ENDL;
		m_messages << "Bottle is displaced by: \t" << - mBottle->get_displacement_correction().x << ", " << - mBottle->get_displacement_correction().y << " mm" << ENDL;

		
		// rewarp the image to align bottle with master image
		cvWarpAffine( mImg, corrected_bottle_img, M, CV_INTER_CUBIC + CV_WARP_FILL_OUTLIERS );


		// (2) find the label and measure its position relative to bottle outline
		if ( ! mLabel->process_image( corrected_bottle_img, ML ) )
			return false;
		m_messages << "Label is rotated by (Rel. to bottle):   \t" << - mLabel->get_rotation_correction() << '°' << ENDL;
		m_messages << "Label is displaced by (Rel. to bottle): \t" << - mLabel->get_displacement_correction().x << ", " << - mLabel->get_displacement_correction().y << " mm" << ENDL;

		// check if variation in label position is within acceptable range
		double a = - mLabel->get_rotation_correction();
		if ( a < -m_rotation_tol || a > m_rotation_tol )
		{
			m_messages << "ITEM REJECTED! - exceeded rotation tolerance." << ENDL;
			item_accepted = false;
		}
		
		double dx = - mLabel->get_displacement_correction().x;
		if ( dx < -m_horiz_displ || dx > m_horiz_displ )
		{
			m_messages << "ITEM REJECTED! - exceeded horizontal displacement tolerance." << ENDL;
			item_accepted = false;
		}
		double dy = - mLabel->get_displacement_correction().y;
		if ( dy < -m_vertical_disp || dy > m_vertical_disp )
		{
			m_messages << "ITEM REJECTED! - exceeded vertical displacement tolerance." << ENDL;
			item_accepted = false;
		}


		// (3) combine the transformation matrices together
		CvMat* MF = cvCreateMat( 3, 3, CV_32FC1 );
		M = convert_3x2_matrix_to_3x3( M );
		cvMatMul( M, ML, MF );


		IplImage* corrected_label_img = cvCreateImage( cvGetSize( mImg ), IPL_DEPTH_8U, 1 );
		assert( corrected_label_img );

		MF = convert_3x3_matrix_to_3x2( MF );
		cvWarpAffine( mImg, corrected_label_img, MF, CV_INTER_CUBIC + CV_WARP_FILL_OUTLIERS );
		cvSetImageROI( corrected_label_img, label_roi );

		
		// (4) check label variations
		if ( ! mVar->process_image( corrected_label_img ) )
			return false;
		double v = mVar->get_variation_score() * 100;
		m_messages << "Label area variation = " << v << "%" << ENDL;

		cvSetImageROI( corrected_label_img, label_roi );
		overlay_result( corrected_label_img, mVar->get_variation_mask(), result );
		m_ResultCtrl.ShowImage( result );

		// check if variation in label is within acceptable range
		if ( v > m_variation )
		{
			m_messages << "ITEM REJECTED! - exceeded allowable label variation." << ENDL << ENDL;
			item_accepted = false;
		}

		// cleanup
		if ( item_accepted )
		{
			m_messages << "ITEM OK." << ENDL << ENDL;
			m_Status.SetStatus(STATUS_OK);
			++accepted;
		}
		else
		{
			++rejected;
			m_Status.SetStatus(STATUS_FAILED);
		}
		SetCounters(BLOCK_1, accepted, total);
		SetCounters(BLOCK_2, rejected, total);
		
		cvReleaseImage( &corrected_bottle_img );
		cvReleaseImage( &corrected_label_img );
		cvReleaseMat( &M );
		cvReleaseMat( &ML );
		cvReleaseMat( &MF );
		cvReleaseImage( &mImg );


		if (m_bStop)
			break;

		if (m_StepThrough)
		{
			while (mStep)
			{
				MSG msg;
				while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				if (m_bStop)
					break;
			}
			mStep=true;
		}
	}

	mpCamera2->close_source();
	return true;
}

void CChecklabelDlg::OnOK() 
{
	CParamDlg dlg;

	UpdateData();

	if (m_sample >= 0 && m_sample < MAX_SAMPLES)
	{
		tmpl_size = TemplateSize[m_sample];
		tmpl_dist = TemplateDistance[m_sample];
		mVideoPath=VideoPath[m_sample];
		mTrainingPath=TrainFileMask[m_sample];
		mTestPath=TestFileMask[m_sample];
	}
	else
	{
		AfxMessageBox("Please select a sample!");
		return;
	}

	CString sSample1, sSample2;
	sSample1.Format("sample%d\\Items", m_sample);
	sSample2 = "sample%d\\Items%d";

	int items=gConfiguration.GetProfileInt(sSample1, "Total", 0);
	if (items)
	{
		for (int i = 0; i < items; i++)
		{
			CRectEx r;
			CString s;
			s.Format(sSample2, m_sample, i);
			r.left   = gConfiguration.GetProfileInt(s, "left", 0);
			r.right  = r.left + gConfiguration.GetProfileInt(s, "right", 0);
			r.top    = gConfiguration.GetProfileInt(s, "top", 0);
			r.bottom = r.top + gConfiguration.GetProfileInt(s, "bottom", 0);
			r.type   = gConfiguration.GetProfileInt(s, "type", 0);
			dlg.PushValue(r);
		}
	}
	else
	{
		AfxMessageBox("Error reading registry!");
	}
	
	dlg.SetPath(mVideoPath, mTrainingPath);
	if (dlg.DoModal() == IDOK)
	{
		mbSetToStart[m_sample] = false;
		int items=dlg.GetNumberOfItems();
		
		if (items)
		{
			CString sSample1, sSample2, s;
			sSample1.Format("sample%d\\Items", m_sample);
			sSample2 = "sample%d\\Items%d";
			
			gConfiguration.WriteProfileInt(sSample1, "Total", items);

			mRoi.empty();
			
			for (int i = 0; i < items; i++)
			{
				CRectEx r = dlg.GetRectAt(i);
				
				r.right-=r.left;
				r.bottom-=r.top;

				s.Format(sSample2, m_sample, i);

				gConfiguration.WriteProfileInt(s, "left",   r.left);
				gConfiguration.WriteProfileInt(s, "right",  r.right);
				gConfiguration.WriteProfileInt(s, "top",    r.top);
				gConfiguration.WriteProfileInt(s, "bottom", r.bottom);
				gConfiguration.WriteProfileInt(s, "type",   r.type);
			
				if (r.type==REX_SEARCH_SIZE_REGION)
					mRoi.push_back( cvRect( r.left, r.top, r.right,  r.bottom ) );
				else if (r.type==REX_LABEL_REGION)
					label_roi = cvRect( r.left, r.top, r.right,  r.bottom );
				else if (r.type==REX_SEARCH_REGION)
					search_roi = cvRect( r.left, r.top, r.right,  r.bottom );
			}
		}
		else
		{
			AfxMessageBox("You need to define a set of regions!");
		}
	}
//	CDialog::OnOK();
}

void CChecklabelDlg::LoadSample(int sample)
{
	CString sSample1, sSample2;
	sSample1.Format("sample%d\\Items", sample);
	sSample2 = "sample%d\\Items%d";

	int items=gConfiguration.GetProfileInt(sSample1, "Total", 0);
	if (items)
	{
		mRoi.empty();
		for (int i = 0; i < items; i++)
		{
			CRectEx r;
			CString s;
			s.Format(sSample2, sample, i);
			r.left   = gConfiguration.GetProfileInt(s, "left", 0);
			r.right  = gConfiguration.GetProfileInt(s, "right", 0);
			r.top    = gConfiguration.GetProfileInt(s, "top", 0);
			r.bottom = gConfiguration.GetProfileInt(s, "bottom", 0);
			r.type   = gConfiguration.GetProfileInt(s, "type", 0);
		
			if (r.type==REX_SEARCH_SIZE_REGION)
				mRoi.push_back( cvRect( r.left, r.top, r.right,  r.bottom ) );
			else if (r.type==REX_LABEL_REGION)
				label_roi = cvRect( r.left, r.top, r.right,  r.bottom );
			else if (r.type==REX_SEARCH_REGION)
				search_roi = cvRect( r.left, r.top, r.right,  r.bottom );
		}
	}
	else
	{
		AfxMessageBox("Error reading registry!");
	}
}

void CChecklabelDlg::OnStepThrough() 
{
	m_startCtrl.EnableWindow(!m_stepThroughCtrl.GetCheck());
	m_stepCtrl.EnableWindow(m_stepThroughCtrl.GetCheck());
}

void CChecklabelDlg::OnStep() 
{
	UpdateData(TRUE);
	if (m_StepThrough && mTrainWasOK)
	{
		mTrainWasOK=false;
		DoStart();
	}
	mStep=false;	
}

void CChecklabelDlg::EnableWin(int seq)
{
	m_sample_1ctrl.EnableWindow(seq & BLOCK_1);
	m_sample_2ctrl.EnableWindow(seq & BLOCK_1);
	m_sample_3ctrl.EnableWindow(seq & BLOCK_1);

	m_debugCtrl.EnableWindow(seq & BLOCK_2);
	m_ParamsCtrl.EnableWindow(seq & BLOCK_2);
	m_trainingCtrl.EnableWindow(seq & BLOCK_2);

	m_stepCtrl.EnableWindow(seq & BLOCK_3);
	m_stepThroughCtrl.EnableWindow(seq & BLOCK_3);
	m_startCtrl.EnableWindow(seq & BLOCK_3);

	if (seq & BLOCK_3)
		OnStepThrough();
	m_vert_dispCtrl.EnableWindow(seq & BLOCK_4);
	m_variationCtrl.EnableWindow(seq & BLOCK_4);
	m_rot_tolCtrl.EnableWindow(seq & BLOCK_4);
	m_horz_dispCtrl.EnableWindow(seq & BLOCK_4);
}

void CChecklabelDlg::OnCancel() 
{
	m_bStop=true;

	Sleep(50);
	
	CDialog::OnCancel();
}

void CChecklabelDlg::OnSample1() 
{
	EnableWin(BLOCK_2|BLOCK_1|BLOCK_4);
}

void CChecklabelDlg::OnSample2() 
{
	EnableWin(BLOCK_2|BLOCK_1|BLOCK_4);
}

void CChecklabelDlg::OnSample3() 
{
	EnableWin(BLOCK_2|BLOCK_1|BLOCK_4);
}

LRESULT CChecklabelDlg::OnCheckDevices(WPARAM, LPARAM) 
{
	m_messages << "Checking connected device. Please wait ...." << ENDL;
	
	for (int i=0; i<10; i++)
	{
		Sleep(200);	

		MSG msg;
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	m_state_1.SetWindowText("OK");
	m_state_2.SetWindowText("OK");
	m_state_3.SetWindowText("OK");
	EnableWin(BLOCK_4|BLOCK_2|BLOCK_1);
	m_messages << "Ready." << ENDL;
	return 0;
}

void CChecklabelDlg::SetCounters(BYTE which, float amount, float perc) 
{
	CString s, p;
	s.Format("%d", (int) amount);
	if (perc < 0.1)
		p.Format("0%%");
	else
		p.Format("%.1f%%", (100.0*float(amount))/perc);

	if(which & BLOCK_1)
	{
		m_count_1.SetWindowText(s);
		m_perc_1.SetWindowText(p);
	}
	if(which & BLOCK_2)
	{
		m_count_2.SetWindowText(s);
		m_perc_2.SetWindowText(p);
	}
	if(which & BLOCK_3)
	{
		m_count_3.SetWindowText(s);
		m_perc_3.SetWindowText(p);
	}
}

void CChecklabelDlg::OnHelp() 
{
	DisplayHelp(0);	
}

void CChecklabelDlg::OnAppAbout() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CAboutDlg::OnAcknowledgements() 
{
	CAcknowledgements ack;
	ack.DoModal();
}
