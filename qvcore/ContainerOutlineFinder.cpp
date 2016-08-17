
#include "ContainerOutlineFinder.h"
#include "coord_geom.h"
///MB:#include <highgui.h>
#include <iostream>
#include <fstream>


using namespace std;




ContainerOutlineFinder::ContainerOutlineFinder()
{
	model_initialised = false;

	// default values
	min_line_fit_threshold = 0.50;
	contour_map_threshold = 0;
	min_line_angle_threshold = 25.0;

	debug = false;
	debug_image = NULL;
	debug_image2 = NULL;

	rotation_correction = 0.0;
	displacement_correction = cvPoint3D32f( 0.0, 0.0, 1.0 );
}



ContainerOutlineFinder::~ContainerOutlineFinder()
{
	if ( debug_image )
		cvReleaseImage( &debug_image );
	if ( debug_image2 )
		cvReleaseImage( &debug_image2 );
}



bool ContainerOutlineFinder::add_model_region( IplImage* contour_map, CvRect& roi )
{
	assert( contour_map );
	assert( ! model_initialised );

	regions.push_back( roi );

	return add_data_region( contour_map, regions.size() -1 );
}



bool ContainerOutlineFinder::add_data_region( IplImage* contour_map, int region_index )
{
	assert( contour_map );

	CvRect& roi = regions[ region_index ];

	if ( debug )
	{
		if ( ! debug_image )
		{
			debug_image = (IplImage*) cvClone( contour_map );
			debug_image2 = (IplImage*) cvClone( contour_map );
		}
		assert( debug_image && debug_image2 );
		cvNamedWindow( "ContainerOutlineFinder-debug1", CV_WINDOW_AUTOSIZE );
		cvNamedWindow( "ContainerOutlineFinder-debug2", CV_WINDOW_AUTOSIZE );

		cvRectangle( debug_image, 
					 cvPoint( roi.x - 1, roi.y - 1 ), 
					 cvPoint( roi.x + 1 + roi.width, roi.y + 1 + roi.height ), 
					 CV_RGB( 128, 128, 128 ) );

		cvSet( debug_image2, CV_RGB( 0, 0, 0 ) );
	}

	line_valid.push_back( false );
	line_eqn.push_back( cvPoint3D32f( 0.0, 0.0, 0.0 ) );
	line_centrept.push_back( cvPoint3D32f( 0.0, 0.0, 0.0 ) );
	line_correlation.push_back( 0.0 );
	line_intersect.push_back( cvPoint3D32f( 0.0, 0.0, 0.0 ) );

	bool res = fit_line_to_region( contour_map, region_index );

	if ( debug )
	{
		cvShowImage( "ContainerOutlineFinder-debug1", debug_image );
		cvShowImage( "ContainerOutlineFinder-debug2", debug_image2 );
	}

	return res;
}



bool ContainerOutlineFinder::finalise_model()
{
	// do we have enough valid regions/lines?
	int num_valid_lines = 0;

	for ( int n = 0; n < regions.size(); ++n )
	{
		if ( line_valid[ n ] )
			++num_valid_lines;
	}

	if ( num_valid_lines < 2 )
	{
		trace_log << "Not enough regions with valid lines in them! Specify more regions." << endl;
		if ( debug )
		{
			cvShowImage( "ContainerOutlineFinder-debug1", debug_image );
			cvWaitKey(0);
		}
		return false;
	}


	// determine intersections beween lines
	if ( ! find_line_intersections() )
	{
		trace_log << "Could not find any intersections between the lines fitted to the container outline!" << endl;
		cvWaitKey(0);
		return false;
	}


	// save model data
	model_initialised = true;
	ref_valid = line_valid;
	ref_eqn = line_eqn;
	ref_centrept = line_centrept;
	ref_correlation = line_correlation;
	ref_intersect = line_intersect;

	
	if ( debug )
	{
		assert( debug_image );
		cvShowImage( "ContainerOutlineFinder-debug1", debug_image );
		cvShowImage( "ContainerOutlineFinder-debug2", debug_image2 );
	}

	return true;
}



bool ContainerOutlineFinder::fit_line_to_region( IplImage* contour_map, int n )
{
	assert( contour_map );
	assert( n >= 0 && n < regions.size() );

	line_valid[ n ] = true;		// assume true initially

	// fit a line to the pixels in the given region using a least-squares method
	if ( ! fit_line_to_image_points( contour_map, regions[ n ], 
									 line_eqn[ n ], line_centrept[ n ], 
									 contour_map_threshold, &( line_correlation[ n ] ) ) )
	{
		trace_log << "No points within the given region!" << endl;
		line_valid[ n ] = false;
		return false;
	}

	if ( fabs( line_correlation[ n ] ) < min_line_fit_threshold )
	{
		trace_log << "Line in region:  " << line_eqn[ n ].x << " x + " << line_eqn[ n ].y << " y + " << line_eqn[ n ].z << " = 0 "
				  << "  thru (" << line_centrept[ n ].x << "," << line_centrept[ n ].y << "," << line_centrept[ n ].z << ")   r=" << line_correlation[ n ] << endl;
		trace_log << "Line fitting is poor within given region" << "  r=" << line_correlation[ n ] << endl;
		line_valid[ n ] = false;
		return false;
	}

	
	if ( debug && line_valid[ n ] )
	{
		trace_log << "Line in region:  " << line_eqn[ n ].x << " x + " << line_eqn[ n ].y << " y + " << line_eqn[ n ].z << " = 0 "
				  << "  thru (" << line_centrept[ n ].x << "," << line_centrept[ n ].y << "," << line_centrept[ n ].z << ")   r=" << line_correlation[ n ] << endl;

		// draw line
		assert( debug_image );
		if ( line_eqn[ n ].y == 0.0 )
		{
			// perfect vertical line
			cvLine( debug_image, 
					cvPoint( line_centrept[ n ].x, 0.0 ), 
					cvPoint( line_centrept[ n ].x, debug_image->height ), 
					CV_RGB(128,128,128) );
		}
		else
		{
			cvLine( debug_image, 
					cvPoint( debug_image->width, ( - line_eqn[ n ].x * debug_image->width - line_eqn[ n ].z ) / line_eqn[ n ].y ), 
					cvPoint( 0.0, - line_eqn[ n ].z / line_eqn[ n ].y ), 
					CV_RGB(128,128,128) );
		}
	}

	return true;
}



bool ContainerOutlineFinder::find_line_intersections()
{
	int num_valid_intersections = 0;

	// consider pairs of lines n, n+1
	for ( int n = 0; n < regions.size(); ++n )
	{
		// the second line
		int n2 = ( n == regions.size() - 1 ? 0 : n + 1 );

		if ( n == 1 && regions.size() == 2 )
			continue;

		if ( line_valid[ n ] && line_valid[ n2 ] )
		{
			// ignore lines which are nearly parallel, as any small error in the
			// line equation will give a large variation in the intersection point.
			double diff = angle_between_lines( line_eqn[ n ], line_eqn[ n2 ] );
			if ( fabs( diff ) < ( min_line_angle_threshold * CV_PI / 180.0 ) )
			{
				line_intersect[ n ].x = line_intersect[ n ].y = line_intersect[ n ].z = 0.0;
				continue;
			}

			// Find pt of intersection between the two lines.
			// Using homogeneous coords, the point of intersection between 2 lines
			// is obtained by taking the cross product (vector product) of the 2 lines.
			line_intersect[ n ] = hc_cross_product( line_eqn[ n ], line_eqn[ n2 ] );

			if ( debug )
			{
				trace_log << "Lines #" << n+1 << "," << n2+1 << " intersect at " 
						  << line_intersect[ n ].x << "," << line_intersect[ n ].y << "," << line_intersect[ n ].z << endl;
			}

			if ( line_intersect[ n ].z == 0.0 )	// point at infinity, i.e. lines intersect
			{									// at infinity, i.e. lines are parallel.
				continue;
			}


			++num_valid_intersections;

			
			if ( debug )
			{
				cvCircle( debug_image, cvPoint( line_intersect[ n ].x, line_intersect[ n ].y ), 5, 
						  CV_RGB( 128,128,128 ) );
			}
		}
	}

	return ( num_valid_intersections >= 1 );
}



bool ContainerOutlineFinder::process_image( IplImage* contour_map, CvMat* registration_matrix )
{
	assert( contour_map );
	assert( registration_matrix );
	assert( registration_matrix->width == 3 && 
			( registration_matrix->height == 2 || registration_matrix->height == 3 ) );

	rotation_correction = 0.0;
	displacement_correction = cvPoint3D32f( 0.0, 0.0, 1.0 );

	if ( debug )
	{
		if ( debug_image )
		{
			cvReleaseImage( &debug_image );
			cvReleaseImage( &debug_image2 );
		}
		debug_image = debug_image2 = NULL;
	}


	if ( ! model_initialised )
	{
		trace_log << "No reference model exists! Must learn a model first." << endl;
		return false;
	}


	line_valid.clear();
	line_eqn.clear();
	line_centrept.clear();
	line_correlation.clear();
	line_intersect.clear();


	// build line data
	int num_valid_lines = 0;

	for ( int n = 0; n < regions.size(); ++n )
		if ( add_data_region( contour_map, n ) )
			++num_valid_lines;

	if ( num_valid_lines < 2 )
	{
		trace_log << "Not enough regions with valid lines in them!" << endl;
		if ( debug )
		{
			cvShowImage( "ContainerOutlineFinder-debug1", debug_image );
			cvWaitKey(0);
		}
		return false;
	}


	// We adopt a stratified approach
	// i.e. first recover rotation difference, and correct for rotation, then
	// recover x-,y- displacement, and correct for displacement.

	// PHASE 1: recover rotation correction
	double total_weight = 0.0;

	for ( n = 0; n < regions.size(); ++n )
	{
		if ( line_valid[ n ] && ref_valid[ n ] )
		{
			// find angle between the 2 lines.
			double diff = angle_between_lines( line_eqn[ n ], ref_eqn[ n ] );

			if ( debug )
			{
				trace_log << "Rotation difference for line of region " << n << " = " << diff * 180 / CV_PI << endl;
			}
					 
			// calculate weighted difference of line angles
			rotation_correction += diff * line_correlation[ n ];
			total_weight += line_correlation[ n ];
		}
	}

	if ( total_weight < min_line_fit_threshold )
	{
		trace_log << "Could not compare container outline to the learnt model outline!" << endl;
		cvWaitKey(0);
		return false;
	}

	rotation_correction /= total_weight;

	if ( debug )
	{
		trace_log << "Weighted rotation difference = " << rotation_correction * 180 / CV_PI << endl;
	}

	if ( fabs( rotation_correction ) < 0.0001 )
		rotation_correction = 0.0;

	// now build the matrix that corrects for rotation. We arbitrarily choose the image
	// centre as the point around which rotation is performed.
	CvPoint3D32f img_centre_pt = cvPoint3D32f( contour_map->width / 2.0, contour_map->height / 2.0, 1.0 );
	CvMat* Mrt = build_euclidean_transform_matrix( cvPoint3D32f( 0, 0, 1 ), rotation_correction, img_centre_pt );
	assert( Mrt );
	
	
	// adjust the line in the current image for rotation
	if ( rotation_correction != 0.0 )
	{
		for ( n = 0; n < regions.size(); ++n )
		{
			if ( line_valid[ n ] )
			{
				// get 2 arbitrary points on the line in current image: we use the region's 
				// boundary to get 2 such points.
				CvPoint3D32f pt1, pt2;
				
				if ( line_eqn[ n ].y == 0.0 )		// vertical line
				{
					pt1.y = regions[ n ].y;
					pt2.y = regions[ n ].y + regions[ n ].height;

					pt1.x = ( - line_eqn[ n ].y * pt1.y - line_eqn[ n ].z ) / line_eqn[ n ].x;
					pt2.x = ( - line_eqn[ n ].y * pt2.y - line_eqn[ n ].z ) / line_eqn[ n ].x;
				}
				else
				{
					pt1.x = regions[ n ].x;
					pt2.x = regions[ n ].x + regions[ n ].width;

					pt1.y = ( - line_eqn[ n ].x * pt1.x - line_eqn[ n ].z ) / line_eqn[ n ].y;
					pt2.y = ( - line_eqn[ n ].x * pt2.x - line_eqn[ n ].z ) / line_eqn[ n ].y;
				}
				pt1.z = pt2.z = 1.0;

				// now rotate these 2 points and the line centroid
				CvPoint3D32f rpt1 = hc_transform_point( pt1, *Mrt );
				CvPoint3D32f rpt2 = hc_transform_point( pt2, *Mrt );
				line_centrept[ n ] = hc_transform_point( line_centrept[ n ], *Mrt );


				// re-estimate line from the rotated points
				// The line between 2 points p1, p2 is obtained by taking the 
				// cross product (vector product) of the 2 points.
				line_eqn[ n ] = cross_product( rpt1, rpt2 );

				
				if ( debug )
				{
					trace_log << "Line corrected for rotation:  " << line_eqn[ n ].x << " x + " << line_eqn[ n ].y << " y + " << line_eqn[ n ].z << " = 0 "
							  << "  thru (" << line_centrept[ n ].x << "," << line_centrept[ n ].y << "," << line_centrept[ n ].z << ")   r=" << line_correlation[ n ] << endl;
				}
			}
		}
	}
	

	// PHASE 2: recover displacement correction.
	
	// re-calculate intersections between (rotationally-corrected) lines 
	if ( ! find_line_intersections() )
	{
		trace_log << "Could not find any intersections between the lines fitted to the container outline!" << endl;
		cvReleaseMat( &Mrt );
		cvWaitKey(0);
		return false;
	}

	total_weight = 0.0;

	for ( n = 0; n < regions.size(); ++n )
	{
		if ( line_valid[ n ] && ref_valid[ n ] &&
			 line_intersect[ n ].z != 0.0 &&
			 ref_intersect[ n ].z != 0.0 )		// not parallel (or nearly parallel) lines
		{
			// find difference between the intersection point of this line and that of ref model
			CvPoint2D32f disp;
			disp.x = ( ref_intersect[ n ].x - line_intersect[ n ].x ) * line_correlation[ n ];
			disp.y = ( ref_intersect[ n ].y - line_intersect[ n ].y ) * line_correlation[ n ];

			if ( debug )
			{
				trace_log << "Displacement for intersection pt of line of region " << n << " = " 
						  << disp.x << "," << disp.y << endl;
			}

			// calculate weighted difference of line intersections
			displacement_correction.x += disp.x * line_correlation[ n ];
			displacement_correction.y += disp.y * line_correlation[ n ];
			total_weight += line_correlation[ n ];
		}
	}

	if ( total_weight < min_line_fit_threshold )
	{
		trace_log << "Could not compare container outline to the learnt model outline!" << endl;
		cvReleaseMat( &Mrt );
		cvWaitKey(0);
		return false;
	}

	displacement_correction.x /= total_weight;
	displacement_correction.y /= total_weight;

	if ( debug )
	{
		trace_log << "Weighted relative displacement = " << displacement_correction.x << "," << displacement_correction.y << "," << displacement_correction.z << endl;
	}


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
	CvPoint3D32f p2 = hc_transform_point( img_centre_pt, *Mrt );
	p2.x /= p2.z;
	p2.y /= p2.z;
	p2.z /= p2.z;
	p2.x = p2.x - img_centre_pt.x;
	p2.y = p2.y - img_centre_pt.y;
	displacement_correction = p2;



	if ( debug )
	{
		Mrt = convert_3x3_matrix_to_3x2( Mrt );
		cvWarpAffine( contour_map, debug_image2, Mrt,
					  CV_INTER_CUBIC + CV_WARP_FILL_OUTLIERS );

		for ( int n = 0; n < regions.size(); ++n )
		{
			if ( line_valid[ n ] )
			{
				if ( line_eqn[ n ].y == 0.0 )
				{
					// perfect vertical line
					cvLine( debug_image, 
							cvPoint( line_centrept[ n ].x + displacement_correction.x, 
									 0.0 + displacement_correction.y ), 
							cvPoint( line_centrept[ n ].x + displacement_correction.x, 
									 debug_image->height + displacement_correction.y ), 
							CV_RGB(50,50,50) );
				}
				else
				{
					cvLine( debug_image, 
							cvPoint( debug_image2->width + displacement_correction.x, 
									 ( - line_eqn[ n ].x * debug_image2->width - line_eqn[ n ].z ) / line_eqn[ n ].y + displacement_correction.y ), 
							cvPoint( 0.0 +  displacement_correction.x, 
									 - line_eqn[ n ].z / line_eqn[ n ].y + displacement_correction.y ), 
							CV_RGB(50,50,50) );
				}
			}
		}

		cvShowImage( "ContainerOutlineFinder-debug1", debug_image );
		cvShowImage( "ContainerOutlineFinder-debug2", debug_image2 );
	}

	cvReleaseMat( &Mrt );

	return true;
}


