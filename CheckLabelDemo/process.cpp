
#include "../qvcore/framework.h"
#include <iostream>
#include <cmath>
#include <cfloat>

using namespace std;

#define TEST_1
//#define TEST_2
//#define TEST_3



void main_function()
{
	cvNamedWindow( "Input", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "Master", CV_WINDOW_AUTOSIZE );
	cvNamedWindow( "Result", CV_WINDOW_AUTOSIZE );


#ifdef TEST_1
#	define VIDEO_PATH	"..\\data\\label_check\\test1\\"
#	define TRAIN_FILE_MASK	"%s\\training\\img%02.02d.bmp"
#	define TEST_FILE_MASK	"%s\\testing\\img%02.02d.bmp"

	// search regions used to measure bottle position
	vector< CvRect > roi;
	roi.push_back( cvRect( 175,	25,		250,	50 ) );
	roi.push_back( cvRect( 30,	110,	50,		140 ) );
	roi.push_back( cvRect( 175,	390,	250,	50 ) );

	// label region and search area
	CvRect label_roi	= cvRect( 110, 105, 410, 250 );
	CvRect search_roi	= cvRect( 70, 70, 480, 320 );

	const int tmpl_size = 41;
	const int tmpl_dist = 75;

#elif defined( TEST_2 )
#	define VIDEO_PATH	"..\\data\\label_check\\test2\\"
#	define TRAIN_FILE_MASK	"%s\\training\\img%02.02d.bmp"
#	define TEST_FILE_MASK	"%s\\testing\\img%02.02d.bmp"
	
	// search regions used to measure bottle position
	vector< CvRect > roi;
	roi.push_back( cvRect( 110,	360,	100,	60 ) );
	roi.push_back( cvRect( 22,	200,	60,		140 ) );

	// label region and search area
	CvRect label_roi	= cvRect( 105, 152, 120, 198 );
	CvRect search_roi	= cvRect( 90, 138, 146, 230 );

	const int tmpl_size = 21;
	const int tmpl_dist = 10;

#elif defined( TEST_3 )
#	define VIDEO_PATH	"..\\data\\label_check\\test3\\"
#	define TRAIN_FILE_MASK	"%s\\training\\img%02.02d.bmp"
#	define TEST_FILE_MASK	"%s\\testing\\img%02.02d.bmp"
	
	// search regions used to measure bottle position
	vector< CvRect > roi;
	roi.push_back( cvRect( 190,	300,	100,	40 ) );
	roi.push_back( cvRect( 30,	295,	100,	40 ) );

	// label region and search area
	CvRect label_roi	= cvRect( 92, 180, 138, 98 );
	CvRect search_roi	= cvRect( 75, 158, 180, 155 );

	const int tmpl_size = 21;
	const int tmpl_dist = 10;

#else
#	error	"Ooops!"
#endif



	//=================== TRAINING ==========================================
	cout << "===== Starting training proces...." << endl;

	NumberedFileCamera camera( VIDEO_PATH, TRAIN_FILE_MASK );
	camera.set_output_greyscale();
	if ( ! camera.open_source() )
		exit(1);

	// get an initial image - will be used as template
	IplImage* img = camera.acquire_image();
	if ( ! img )
		exit(1);
	
	cvShowImage( "Master", img );
	cout << "===== Press any key to start Training process...." << endl;
	cvWaitKey( 0 );
	cout << "Training started.... please wait; this might take several minutes..." << endl;


	// (1) train the algorithm to find the bottle outline 
	ContainerOutlineFinder bottle;
	///bottle.set_debug();

	// find edge contours
	IplImage* edge_map = cvCreateImage( cvGetSize( img ), IPL_DEPTH_8U, 1 );
	assert( edge_map );
	cvCanny( img, edge_map, 30, 100 );


	// the user adds the regions containing linear parts of the outline of the bottle
	for ( int n = 0; n < roi.size(); ++n )
		if ( ! bottle.add_model_region( edge_map, roi[ n ] ) )
			cout << "This region will not be used." << endl;


	// finalise the bottle outline model
	if ( ! bottle.finalise_model() )
		exit(1);

	

	// (2) training the program to find the label area

	// find regions of strong texture inside the label that can be used to locate the label
	HighTexturedAreaFinder texture;
	///texture.set_debug();
	texture.set_search_area( label_roi );
	texture.set_min_template_distance( tmpl_dist );
	texture.set_template_width( tmpl_size );
	texture.set_template_height( tmpl_size );
	const int max_search_areas = 4;

	bool ok = texture.process_image( img );
	if ( ! ok )
	{
		cout << "Error initialising texture search!" << endl;
		exit(1);
	}


	ContainerAreaFinder label;
	///label.set_debug();
	label.set_label_area( label_roi );
	label.set_search_area( search_roi );

	// add the areas of strong texture found above to the label finder object
	do
	{
		CvRect area;
		ok = false;
		if ( label.num_templates() < max_search_areas && 
			 ( ok = texture.find_strong_textured_area( area ) ) )
			label.add_template_area( area );
	}
	while( ok );

	if ( label.num_templates() < 3 )
	{
		cout << "Less than 3 label search areas found!" << endl;
		exit(1);
	}


	// learn the label model
	label.finalise_model( img );


	// (3) training the program to learn variations in the label area
	AreaVarianceDetector var;
	var.set_area( label_roi );
	///var.set_debug();

	var.learn_sample( img );

	
	///cvWaitKey( 0 );
	cvReleaseImage( &img );


	while( ( img = camera.acquire_image() ) != NULL )
	{
		cvShowImage( "Input", img );


		// find edge contours
		assert( edge_map );
		cvCanny( img, edge_map, 30, 100 );

		
		// (3a) find and correct bottle outline
		CvMat* M = cvCreateMat( 2, 3, CV_32FC1 );
		CvMat* ML = cvCreateMat( 3, 3, CV_32FC1 );
		assert( M && ML );

		IplImage* corrected_bottle_img = cvCreateImage( cvGetSize( img ), IPL_DEPTH_8U, 1 );
		assert( corrected_bottle_img );

		if ( ! bottle.process_image( edge_map, M ) )
			exit(1);
		cout << "BOTTLE IS ROTATED BY:   " << - bottle.get_rotation_correction() << endl;
		cout << "BOTTLE IS DISPLACED BY: " << - bottle.get_displacement_correction().x << "," << - bottle.get_displacement_correction().y << endl;

		
		// rewarp the image to align bottle with master image
		cvWarpAffine( img, corrected_bottle_img, M, CV_INTER_CUBIC + CV_WARP_FILL_OUTLIERS );


		// (3b) find the label and measure its position relative to bottle outline
		if ( ! label.process_image( corrected_bottle_img, ML ) )
			exit(1);
		cout << "LABEL IS ROTATED BY (REL. TO BOTTLE):   " << - label.get_rotation_correction() << endl;
		cout << "LABEL IS DISPLACED BY (REL. TO BOTTLE): " << - label.get_displacement_correction().x << "," << - label.get_displacement_correction().y << endl;


		// (3c) combine the transformation matrices together
		CvMat* MF = cvCreateMat( 3, 3, CV_32FC1 );
		M = convert_3x2_matrix_to_3x3( M );
		cvMatMul( M, ML, MF );


		IplImage* corrected_label_img = cvCreateImage( cvGetSize( img ), IPL_DEPTH_8U, 1 );
		assert( corrected_label_img );

		MF = convert_3x3_matrix_to_3x2( MF );
		cvWarpAffine( img, corrected_label_img, MF, CV_INTER_CUBIC + CV_WARP_FILL_OUTLIERS );


		// (3d) learn label variations
		var.learn_sample( corrected_label_img );



		// cleanup
		///cvWaitKey( 0 );
		cvReleaseImage( &corrected_bottle_img );
		cvReleaseImage( &corrected_label_img );
		cvReleaseMat( &M );
		cvReleaseMat( &ML );
		cvReleaseMat( &MF );
		cvReleaseImage( &img );
		cout << endl;
	}


	// (3e) finalise label variation model
	var.finalise_model();


	// cleanup
	cout << "===== Training proces finished." << endl;
	cout << "===== Press any key to start Online Testing of product...." << endl;
	cvWaitKey( 0 );
	cvReleaseImage( &img );
	camera.close_source();



	//=================== ONLINE TESTING ==========================================

	NumberedFileCamera camera2( VIDEO_PATH, TEST_FILE_MASK );
	camera2.set_output_greyscale();
	if ( ! camera2.open_source() )
		exit(1);

	IplImage* result = cvCreateImage( cvSize( label_roi.width, label_roi.height ), IPL_DEPTH_8U, 3 );

	while( ( img = camera2.acquire_image() ) != NULL )
	{
		cvShowImage( "Input", img );
		bool item_accepted = true;


		// find edge contours
		assert( edge_map );
		cvCanny( img, edge_map, 30, 100 );

		
		// (1) find and correct bottle outline
		CvMat* M = cvCreateMat( 2, 3, CV_32FC1 );
		CvMat* ML = cvCreateMat( 3, 3, CV_32FC1 );
		assert( M && ML );

		IplImage* corrected_bottle_img = cvCreateImage( cvGetSize( img ), IPL_DEPTH_8U, 1 );
		assert( corrected_bottle_img );

		if ( ! bottle.process_image( edge_map, M ) )
			exit(1);
		cout << "BOTTLE IS ROTATED BY:   " << - bottle.get_rotation_correction() << endl;
		cout << "BOTTLE IS DISPLACED BY: " << - bottle.get_displacement_correction().x << "," << - bottle.get_displacement_correction().y << endl;

		
		// rewarp the image to align bottle with master image
		cvWarpAffine( img, corrected_bottle_img, M, CV_INTER_CUBIC + CV_WARP_FILL_OUTLIERS );


		// (2) find the label and measure its position relative to bottle outline
		if ( ! label.process_image( corrected_bottle_img, ML ) )
			exit(1);
		cout << "LABEL IS ROTATED BY (REL. TO BOTTLE):   " << - label.get_rotation_correction() << endl;
		cout << "LABEL IS DISPLACED BY (REL. TO BOTTLE): " << - label.get_displacement_correction().x << "," << - label.get_displacement_correction().y << endl;

		// check if variation in label position is within acceptable range
		double a = - label.get_rotation_correction();
		if ( a < -3 || a > 3 )
		{
			cout << "ITEM REJECTED! - exceeded rotation tolerance." << endl;
			item_accepted = false;
		}
		double dx = - label.get_displacement_correction().x;
		if ( dx < -10 || dx > 10 )
		{
			cout << "ITEM REJECTED! - exceeded horizontal displacement tolerance." << endl;
			item_accepted = false;
		}
		double dy = - label.get_displacement_correction().y;
		if ( dy < -5 || dy > 5 )
		{
			cout << "ITEM REJECTED! - exceeded vertical displacement tolerance." << endl;
			item_accepted = false;
		}


		// (3) combine the transformation matrices together
		CvMat* MF = cvCreateMat( 3, 3, CV_32FC1 );
		M = convert_3x2_matrix_to_3x3( M );
		cvMatMul( M, ML, MF );


		IplImage* corrected_label_img = cvCreateImage( cvGetSize( img ), IPL_DEPTH_8U, 1 );
		assert( corrected_label_img );

		MF = convert_3x3_matrix_to_3x2( MF );
		cvWarpAffine( img, corrected_label_img, MF, CV_INTER_CUBIC + CV_WARP_FILL_OUTLIERS );
		cvSetImageROI( corrected_label_img, label_roi );

		
		// (4) check label variations
		if ( ! var.process_image( corrected_label_img ) )
			exit(1);
		double v = var.get_variation_score() * 100;
		cout << "LABEL VARIATION = " << v << "%" << endl;

		cvSetImageROI( corrected_label_img, label_roi );
		overlay_result( corrected_label_img, var.get_variation_mask(), result );
		cvShowImage( "Result", result );

		// check if variation in label is within acceptable range
		if ( v > 0.05 )
		{
			cout << "ITEM REJECTED! - exceeded allowable label variation." << endl;
			item_accepted = false;
		}


		// cleanup
		if ( item_accepted )
			cout << "ITEM OK." << endl;
		cvWaitKey( 0 );
		cvReleaseImage( &corrected_bottle_img );
		cvReleaseImage( &corrected_label_img );
		cvReleaseMat( &M );
		cvReleaseMat( &ML );
		cvReleaseMat( &MF );
		cvReleaseImage( &img );
		cout << endl;
	}

	camera2.close_source();
}
