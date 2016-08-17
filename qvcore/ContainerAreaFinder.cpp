
#include "ContainerAreaFinder.h"
#include "coord_geom.h"
///MB:#include <highgui.h>
#include <iostream>


using namespace std;



ContainerAreaFinder::ContainerAreaFinder()
{
	model_initialised = false;

	debug = false;
	debug_image = NULL;
	debug_image2 = NULL;
	debug_image3 = NULL;

	search_roi = cvRect( 0, 0, 1, 1 );
	label_roi = cvRect( 0, 0, 1, 1 );

	// default values
	min_line_angle_threshold = 10.0;

	rotation_correction = 0.0;
	displacement_correction = cvPoint3D32f( 0.0, 0.0, 1.0 );
}



bool ContainerAreaFinder::add_template_area( CvRect& roi )
{
	assert( ! model_initialised );

	if ( template_roi.size() != 0 )
		if ( roi.width != template_roi[ 0 ].width ||
			 roi.height != template_roi[ 0 ].height )
		{
			trace_log << "Templates must be of the same size!" << endl;
			return false;
		}

	for ( int n = 0; n < template_roi.size(); ++n )
		if ( fabs( roi.x - template_roi[ n ].x ) < 1 &&
			 fabs( roi.y - template_roi[ n ].y ) < 1 )
		{
			trace_log << "Error adding new template. It has the same position as a previous one!" << endl;
			return false;
		}

	template_roi.push_back( roi );
	return true;
}



ContainerAreaFinder::~ContainerAreaFinder()
{
	if ( debug_image != NULL )
		cvReleaseImage( &debug_image );
	if ( debug_image2 != NULL )
		cvReleaseImage( &debug_image2 );
	if ( debug_image3 != NULL )
		cvReleaseImage( &debug_image3 );

	for ( int n = 0; n < label_templates.size(); ++n )
	{
		assert( label_templates[ n ] != NULL );
		cvReleaseImage( &( label_templates[ n ] ) );
	}
	for ( n = 0; n < wrk.size(); ++n )
	{
		assert( wrk[ n ] != NULL );
		cvReleaseImage( &( wrk[ n ] ) );
	}
}




bool ContainerAreaFinder::finalise_model( IplImage* image )
{
	assert( image );
	assert( ! model_initialised );
	assert( template_roi.size() > 0 );
	assert( search_roi.width > template_roi[ 0 ].width && 
			search_roi.height > template_roi[ 0 ].height );
	assert( search_roi.width <= image->width && 
			search_roi.height <= image->height );
	assert( label_templates.size() == 0 );		// must only be called once


	// search area must contain the label area completely
	if ( search_roi.x > label_roi.x || search_roi.y > label_roi.y ||
		 ( search_roi.x + search_roi.width ) < ( label_roi.x + label_roi.width ) ||
		 ( search_roi.y + search_roi.height ) < ( label_roi.y + label_roi.height ) )
	{
		trace_log << "Label area is outside of Search area!" << endl;
		return false;
	}


	if ( debug )
	{
		if ( debug_image != NULL )
			cvReleaseImage( &debug_image );
		debug_image = cvCloneImage( image );
		assert( debug_image );

		cvRectangle( debug_image, 
					 cvPoint( search_roi.x - 1, search_roi.y - 1 ), 
					 cvPoint( search_roi.x + 1 + search_roi.width, 
							  search_roi.y + 1 + search_roi.height ), 
					 CV_RGB( 128, 128, 128 ) );

		for ( int n = 0; n < template_roi.size(); ++n )
			cvRectangle( debug_image, 
						 cvPoint( template_roi[ n ].x - 1, template_roi[ n ].y - 1 ), 
						 cvPoint( template_roi[ n ].x + 1 + template_roi[ n ].width, 
								  template_roi[ n ].y + 1 + template_roi[ n ].height ), 
						 CV_RGB( 128, 128, 128 ) );

		cvNamedWindow( "ContainerAreaFinder-debug", CV_WINDOW_AUTOSIZE );
		cvShowImage( "ContainerAreaFinder-debug", debug_image );
	}

	
	// do we have enough templates?
	if ( template_roi.size() < 3 )
	{
		trace_log << "Not enough templates given! Specify at least 3 templates." << endl;
		return false;
	}


	// store the template images
	for ( int n = 0; n < template_roi.size(); ++n )
	{
		IplImage* tmpl = cvCreateImage( cvSize( template_roi[ n ].width, template_roi[ n ].height ), IPL_DEPTH_8U, 1 );
		assert( tmpl );

		CvRect old_rect = cvGetImageROI( image );
		cvSetImageROI( image, template_roi[ n ] );
		cvCopy( image, tmpl );
		cvSetImageROI( image, old_rect );

		assert( tmpl->width == template_roi[ n ].width && tmpl->height == template_roi[ n ].height );

		label_templates.push_back( tmpl );

		if ( debug )
		{
			trace_log << "Template " << n << " @ " << template_roi[ n ].x << "," << template_roi[ n ].y << endl;
		}
	}


	// fit lines to positions of consecutive pairs of templates n, n+1
	for ( n = 0; n < template_roi.size(); ++n )
	{
		// the second template
		int n2 = ( n == template_roi.size() - 1 ? 0 : n + 1 );

		if ( n == 1 && template_roi.size() == 2 )
			continue;

		CvPoint3D32f p1, p2;
		p1.x = template_roi[ n ].x;
		p1.y = template_roi[ n ].y;
		p1.z = 1.0;

		p2.x = template_roi[ n2 ].x;
		p2.y = template_roi[ n2 ].y;
		p2.z = 1.0;

		// Find the line joining the 2 points.
		// Using homogeneous coords, the line passing through 2 points
		// is obtained by taking the cross product (vector product) of the 2 points.
		CvPoint3D32f ln = hc_cross_product( p1, p2 );
		ref_eqn.push_back( ln );

		if ( debug )
		{
			cvLine( debug_image, cvPoint( p1.x, p1.y ), cvPoint( p2.x, p2.y ), CV_RGB( 128, 128, 128 ) );
			trace_log << "Line " << n << " := " << ref_eqn[ n ].x << "," << ref_eqn[ n ].y << "," << ref_eqn[ n ].z << endl;
		}
	}


	// do some pre-allocation to make the matching part faster
	for ( n = 0; n < template_roi.size(); ++n )
	{
		// determine search area for each template:
		// we limit our template search area by requiring the label area to be completely 
		// within the global search area and knowing the position of the template with 
		// respect to the global label area.
		CvRect search = search_roi;
		search.x += template_roi[ n ].x - label_roi.x;
		search.y += template_roi[ n ].y - label_roi.y;
		search.width -= label_roi.width - template_roi[ n ].width;
		search.height -= label_roi.height - template_roi[ n ].height;
		roi.push_back( search );

		IplImage* result = cvCreateImage( cvSize( search.width - label_templates[ n ]->width +1, 
												  search.height - label_templates[ n ]->height +1 ),
										  IPL_DEPTH_32F, 1 );
		assert( result );
		wrk.push_back( result );
	}


	model_initialised = true;


	if ( debug )
	{
		cvShowImage( "ContainerAreaFinder-debug", debug_image );
	}

	return true;
}



bool ContainerAreaFinder::process_image( IplImage* image, CvMat* registration_matrix )
{
	assert( image );
	assert( model_initialised );
	assert( label_templates.size() > 0 );
	assert( search_roi.width > template_roi[ 0 ].width && 
			search_roi.height > template_roi[ 0 ].height );
	assert( search_roi.width <= image->width && 
			search_roi.height <= image->height );
	assert( registration_matrix->width == 3 && 
			( registration_matrix->height == 2 || registration_matrix->height == 3 ) );

	rotation_correction = 0.0;
	displacement_correction = cvPoint3D32f( 0.0, 0.0, 1.0 );


	if ( debug )
	{
		if ( debug_image2 != NULL )
			cvReleaseImage( &debug_image2 );
		debug_image2 = cvCloneImage( image );
		assert( debug_image2 );

		if ( debug_image3 != NULL )
			cvReleaseImage( &debug_image3 );
		debug_image3 = cvCloneImage( image );
		assert( debug_image3 );

		cvRectangle( debug_image2, 
					 cvPoint( search_roi.x - 1, search_roi.y - 1 ), 
					 cvPoint( search_roi.x + 1 + search_roi.width, 
							  search_roi.y + 1 + search_roi.height ), 
					 CV_RGB( 128, 128, 128 ) );

		cvNamedWindow( "ContainerAreaFinder-debug2", CV_WINDOW_AUTOSIZE );
		cvNamedWindow( "ContainerAreaFinder-debug3", CV_WINDOW_AUTOSIZE );
		cvShowImage( "ContainerAreaFinder-debug2", debug_image2 );
	}


	vector< CvPoint3D32f > template_pos;
	vector< CvPoint3D32f > line_eqn;
	vector< double > template_score;

	template_pos.resize( label_templates.size() );
	template_score.resize( label_templates.size() );
	line_eqn.resize( label_templates.size() );
	

	// remember image's ROI
	CvRect old_rect = cvGetImageROI( image );


	// for each template...
	for ( int n = 0; n < label_templates.size(); ++n )
	{
		cvSetImageROI( image, roi[ n ] );

		// search by normalised cross-correlation
		cvMatchTemplate( image, label_templates[ n ], wrk[ n ], CV_TM_CCORR_NORMED );

		// find the best point
		CvPoint dummy1, pt;
		double dummy2;
		cvMinMaxLoc( wrk[ n ], &dummy2, &template_score[ n ], &dummy1, &pt );
		template_pos[ n ].x = pt.x;
		template_pos[ n ].y = pt.y;
		template_pos[ n ].z = 1.0;

		// adjust for search area; returned positions are relative to search area
		template_pos[ n ].x += roi[ n ].x;
		template_pos[ n ].y += roi[ n ].y;
		

		if ( debug )
		{
			cvRectangle( debug_image2, 
						 cvPoint( template_pos[ n ].x - 1, template_pos[ n ].y - 1 ), 
						 cvPoint( template_pos[ n ].x + 1 + label_templates[0]->width, 
								  template_pos[ n ].y + 1 + label_templates[0]->height ), 
						 CV_RGB( 128, 128, 128 ) );
			trace_log << "Template " << n << " @ " << template_pos[ n ].x << "," << template_pos[ n ].y << endl;
		}
	}


	// restore image's old ROI
	cvSetImageROI( image, old_rect );


	// fit lines to positions of consecutive pairs of templates n, n+1
	for ( n = 0; n < template_roi.size(); ++n )
	{
		// the second template
		int n2 = ( n == template_roi.size() - 1 ? 0 : n + 1 );

		if ( n == 1 && template_roi.size() == 2 )
			continue;

		// Find the line joining the 2 points.
		// Using homogeneous coords, the line passing through 2 points
		// is obtained by taking the cross product (vector product) of the 2 points.
		line_eqn[ n ] = hc_cross_product( template_pos[ n ], template_pos[ n2 ] );

		if ( debug )
		{
			trace_log << "Line " << n << " := " << line_eqn[ n ].x << "," << line_eqn[ n ].y << "," << line_eqn[ n ].z << endl;
		}
	}


	// We adopt a stratified approach
	// i.e. first recover rotation difference, and correct for rotation, then
	// recover x-,y- displacement, and correct for displacement.

	// PHASE 1: recover rotation correction
	for ( n = 0; n < line_eqn.size(); ++n )
	{
		// find angle between the 2 lines.
		double diff = angle_between_lines( line_eqn[ n ], ref_eqn[ n ] );

		if ( debug )
		{
			trace_log << "Rotation difference for line " << n << " = " << diff * 180 / CV_PI << endl;
		}
				 
		// calculate weighted difference of line angles
		rotation_correction += diff;
	}

	assert( line_eqn.size() > 0 );
	rotation_correction /= line_eqn.size();

	if ( debug )
	{
		trace_log << "Weighted rotation difference = " << rotation_correction * 180 / CV_PI << endl;
	}

	if ( fabs( rotation_correction ) < 0.0001 )
		rotation_correction = 0.0;

	
	// now build the matrix that corrects for rotation. We arbitrarily choose the search_roi
	// centre as the point around which rotation is performed.
	CvPoint3D32f rot_centre_pt = cvPoint3D32f( search_roi.x + search_roi.width / 2, 
											   search_roi.y + search_roi.height / 2, 1.0 );
	CvMat* Mrt = build_euclidean_transform_matrix( cvPoint3D32f( 0, 0, 1 ), rotation_correction, rot_centre_pt );
	assert( Mrt );


	// adjust the template positions in the current image for rotation
	if ( rotation_correction != 0.0 )
	{
		for ( n = 0; n < template_roi.size(); ++n )
		{
			CvPoint3D32f rpt = hc_transform_point( template_pos[ n ], *Mrt );
			template_pos[ n ] = rpt;
			
			if ( debug )
				trace_log << "Rotated Template " << n << " @ " << template_pos[ n ].x << "," << template_pos[ n ].y << endl;
		}
	}


	// PHASE 2: recover displacement correction.
	
	for ( n = 0; n < template_roi.size(); ++n )
	{
		// find difference between template pos and that of ref model
		CvPoint2D32f disp;
		disp.x = template_roi[ n ].x - template_pos[ n ].x;
		disp.y = template_roi[ n ].y - template_pos[ n ].y;

		if ( debug )
		{
			trace_log << "Template " << n << " displacement = " 
					  << disp.x << "," << disp.y << endl;
		}

		// calculate weighted difference of line intersections
		displacement_correction.x += disp.x;
		displacement_correction.y += disp.y;
	}

	displacement_correction.x /= template_roi.size();
	displacement_correction.y /= template_roi.size();


	// now build the matrix to perform image registration
	cvmSet( Mrt, 0, 2, cvmGet( Mrt, 0, 2 ) + displacement_correction.x );
	cvmSet( Mrt, 1, 2, cvmGet( Mrt, 1, 2 ) + displacement_correction.y );

	// return matrix 
	if ( registration_matrix->height == 2 )
	{
		for ( int y = 0; y < 2; ++y )
			for ( int x = 0; x < 3; ++x )
				cvmSet( registration_matrix, y, x, cvmGet( Mrt, y, x ) );
	}
	else
	{
		for ( int y = 0; y < 3; ++y )
			for ( int x = 0; x < 3; ++x )
				cvmSet( registration_matrix, y, x, cvmGet( Mrt, y, x ) );
	}


	// since the displacement was calculated after the rotation, it is relative to the
	// rotation-corrected image. We need the absolute displacement so it can be returned
	// to the user. The point around which we did the rotation should remain unchanged by
	// the rotation, and should only be influenced by the absolute displacement.
	CvPoint3D32f p2 = hc_transform_point( rot_centre_pt, *Mrt );
	p2.x /= p2.z;
	p2.y /= p2.z;
	p2.z /= p2.z;
	p2.x = p2.x - rot_centre_pt.x;
	p2.y = p2.y - rot_centre_pt.y;
	displacement_correction = p2;


	if ( debug )
	{
		trace_log << "Weighted relative displacement = " << displacement_correction.x << "," << displacement_correction.y << "," << displacement_correction.z << endl;

		Mrt = convert_3x3_matrix_to_3x2( Mrt );
		cvWarpAffine( debug_image2, debug_image3, Mrt );

		cvShowImage( "ContainerAreaFinder-debug2", debug_image2 );
		cvShowImage( "ContainerAreaFinder-debug3", debug_image3 );
	}

	cvReleaseMat( &Mrt );

	return true;
}




