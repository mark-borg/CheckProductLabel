
#include "HighTexturedAreaFinder.h"
#include "coord_geom.h"
///MB:#include <highgui.h>
#include <iostream>


using namespace std;



HighTexturedAreaFinder::HighTexturedAreaFinder()
{
	template_height = template_width = 31;
	min_template_distance = 20;
	min_area_strength = 0.1;

	eigen_map = NULL;
	
	debug = false;
	debug_image = NULL;
}



bool HighTexturedAreaFinder::set_template_width( int width )
{
	template_width = width;
	if ( template_width % 2 == 0 )
	{
		trace_log << "Invalid template width. Use an odd-numbered length." << endl;
		return false;
	}

	return true;
}



bool HighTexturedAreaFinder::set_template_height( int height )
{
	template_height = height;
	if ( template_height % 2 == 0 )
	{
		trace_log << "Invalid template height. Use an odd-numbered length." << endl;
		return false;
	}

	return true;
}



HighTexturedAreaFinder::~HighTexturedAreaFinder()
{
	if ( eigen_map != NULL )
		cvReleaseImage( &eigen_map );
	if ( debug_image != NULL )
		cvReleaseImage( &debug_image );
}



bool HighTexturedAreaFinder::process_image( IplImage* image )
{
	assert( image );

	if ( eigen_map != NULL )
		cvReleaseImage( &eigen_map );

	// find corner features
	eigen_map = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );
	cvCornerMinEigenVal( image, eigen_map, 3 );

	if ( debug )
	{
		if ( debug_image != NULL )
			cvReleaseImage( &debug_image );
		debug_image = cvCloneImage( eigen_map );
		assert( debug_image );

		cvRectangle( debug_image, 
					 cvPoint( search_roi.x - 1, search_roi.y - 1 ), 
					 cvPoint( search_roi.x + 1 + search_roi.width, search_roi.y + 1 + search_roi.height ), 
					 CV_RGB( 128, 128, 128 ) );

		cvNamedWindow( "HighTexturedAreaFinder-debug", CV_WINDOW_AUTOSIZE );
		cvShowImage( "HighTexturedAreaFinder-debug", debug_image );
	}

	return true;
}



bool HighTexturedAreaFinder::find_strong_textured_area( CvRect& area )
{
	const float IGNORED_AREA = -1e9f;

	// if below fails, you have not called process_image() first
	assert( eigen_map );

	assert( template_width > 0 && template_height > 0 );

	// if below fails, you have forgotten to call set_search_area()
	assert( search_roi.width > template_width && search_roi.height > template_height );

	if ( debug )
	{
		trace_log << "Searching for a strong textured area...." << endl;
	}

	// traverse search_roi and find area with the most and strongest corner features
	double best_strength = 0.0;
	CvPoint best_pos = cvPoint( -1, -1 );


	// do an exhaustive search 
	for ( int y = search_roi.y; y < ( search_roi.y + search_roi.height - template_height ); ++y )
	{
		for ( int x = search_roi.x; x < ( search_roi.x + search_roi.width - template_width ); ++x )
		{
			// get strength of this window
			double strength = 0.0;
			bool skip = false;
			for ( int h = 0; h < template_height && ! skip; ++h )
			{
				float* ptr = (float*)( eigen_map->imageData + ( y + h ) * eigen_map->widthStep + x * sizeof( float ) );
				
				for ( int w = 0; w < template_width && ! skip; ++w, ++ptr )
				{
					if ( fabs( *ptr - IGNORED_AREA ) < 1e-9 )
					{
						// within or near an already-selected area - skip.
						strength = 0.0;
						skip = true;
					}
					else
					{
						strength += fabs( *ptr );
					}
				}
			}

			if ( ! skip )
			{
				strength /= template_width * template_height;	// normalise by area

				// is this the best so far?
				if ( strength > best_strength )
				{
					best_strength = strength;
					best_pos = cvPoint( x, y );
				}
			}
		}
	}

	// anything found?
	if ( best_pos.x != -1 && best_pos.y != -1 && best_strength > min_area_strength )
	{
		area = cvRect( best_pos.x, best_pos.y, template_width, template_height );

		if ( debug )
		{
			assert( debug_image );
			cvRectangle( debug_image, 
						 cvPoint( area.x - 1, area.y - 1 ), 
						 cvPoint( area.x + 1 + area.width, area.y + 1 + area.height ), 
						 CV_RGB( 128, 128, 128 ) );
			cvShowImage( "HighTexturedAreaFinder-debug", debug_image );
		}


		// once an area is found, we blank out this area in the eigen_map, so that it
		// won't be found again in a second call to this function. We also add the 
		// min_template_distance to the area being blanked out.
		cvSetImageROI( eigen_map, cvRect( area.x - min_template_distance,
										  area.y - min_template_distance,
										  area.width + 2 * min_template_distance,
										  area.height + 2 * min_template_distance ) );
		cvSet( eigen_map, cvScalar( IGNORED_AREA ) );
		cvResetImageROI( eigen_map );

		return true;
	}

	return false;
}



