
#include "AreaVarianceDetector.h"
///MB:#include <highgui.h>
#include <iostream>


using namespace std;




AreaVarianceDetector::AreaVarianceDetector()
{
	model_initialised = false;

	debug = false;
	debug_image = NULL;
	debug_image2 = NULL;

	area_roi = cvRect( 0, 0, 0, 0 );
	num_samples = 0;
	
	lum_mean = lum_dev = grad_mean = grad_dev = diff = NULL;
	variation_score = 0.0;

	// default values
	num_sigmas = 3.5;
	mask_size = 5;

	wrk1 = wrk2 = grad_image = NULL;
}



AreaVarianceDetector::~AreaVarianceDetector()
{
	if ( debug_image != NULL )
		cvReleaseImage( &debug_image );
	if ( debug_image2 != NULL )
		cvReleaseImage( &debug_image2 );

	if ( lum_mean != NULL )
		cvReleaseImage( &lum_mean );
	if ( lum_dev != NULL )
		cvReleaseImage( &lum_dev );
	if ( grad_mean != NULL )
		cvReleaseImage( &grad_mean );
	if ( grad_dev != NULL )
		cvReleaseImage( &grad_dev );
	if ( diff != NULL )
		cvReleaseImage( &diff );

	if ( wrk1 != NULL )
		cvReleaseImage( &wrk1 );
	if ( wrk2 != NULL )
		cvReleaseImage( &wrk2 );
	if ( grad_image != NULL )
		cvReleaseImage( &grad_image );
}



bool AreaVarianceDetector::learn_sample( IplImage* image )
{
	assert( ! model_initialised );
	assert( image );
	assert( area_roi.width > 0 && area_roi.height > 0 );
	assert( area_roi.width <= image->width && 
			area_roi.height <= image->height );

	if ( debug )
	{
		if ( debug_image == NULL )
			debug_image = cvCreateImage( cvSize( area_roi.width, area_roi.height ), IPL_DEPTH_8U, 1 );
		assert( debug_image );
		CvRect old_rect = cvGetImageROI( image );
		cvSetImageROI( image, area_roi );
		cvCopy( image, debug_image );
		cvSetImageROI( image, old_rect );

		cvNamedWindow( "AreaVarianceDetector-debug", CV_WINDOW_AUTOSIZE );
		cvShowImage( "AreaVarianceDetector-debug", debug_image );
	}

	
	// generate gradient image
	if ( wrk1 == NULL )
	{
		wrk1 = cvCreateImage( cvSize( area_roi.width, area_roi.height ), IPL_DEPTH_8U, 1 );
		wrk2 = cvCreateImage( cvSize( area_roi.width, area_roi.height ), IPL_DEPTH_16S, 1 );
		grad_image = cvCreateImage( cvSize( area_roi.width, area_roi.height ), IPL_DEPTH_8U, 1 );
	}
	assert( wrk1 && wrk2 && grad_image );
	
	CvRect old_rect = cvGetImageROI( image );
	cvSetImageROI( image, area_roi );
	cvSmooth( image, wrk1, CV_GAUSSIAN, mask_size, mask_size );
	cvLaplace( wrk1, wrk2, mask_size );
	cvConvertScaleAbs( wrk2, grad_image );
	cvSetImageROI( image, old_rect );

	
	++num_samples;

	if ( lum_mean == NULL )
	{
		assert( lum_dev == NULL && grad_mean == NULL && grad_dev == NULL && diff == NULL );
		lum_mean = cvCreateImage( cvSize( area_roi.width, area_roi.height ), IPL_DEPTH_64F, 1 );
		lum_dev  = cvCreateImage( cvSize( area_roi.width, area_roi.height ), IPL_DEPTH_64F, 1 );
		grad_mean = cvCreateImage( cvSize( area_roi.width, area_roi.height ), IPL_DEPTH_64F, 1 );
		grad_dev  = cvCreateImage( cvSize( area_roi.width, area_roi.height ), IPL_DEPTH_64F, 1 );
		diff = cvCreateImage( cvSize( area_roi.width, area_roi.height ), IPL_DEPTH_8U, 1 );
		assert( lum_mean && lum_dev && grad_mean && grad_dev && diff );
		cvSet( lum_mean, cvScalar( 0.0 ) );
		cvSet( lum_dev, cvScalar( 0.0 ) );
		cvSet( grad_mean, cvScalar( 0.0 ) );
		cvSet( grad_dev, cvScalar( 0.0 ) );
		cvSet( diff, cvScalar( 0 ) );
	}
	assert( lum_mean->width == area_roi.width && lum_mean->height == area_roi.height );


	for ( int y = 0; y < area_roi.height; ++y )
	{
		unsigned char* pLumImg  = (unsigned char*)( image->imageData + ( y + area_roi.y ) * image->widthStep + area_roi.x );
		unsigned char* pGradImg = (unsigned char*)( grad_image->imageData + y * grad_image->widthStep );
		double* pLumMean  = (double*)( lum_mean->imageData + y * lum_mean->widthStep );
		double* pLumDev   = (double*)( lum_dev->imageData + y * lum_dev->widthStep );
		double* pGradMean = (double*)( grad_mean->imageData + y * grad_mean->widthStep );
		double* pGradDev  = (double*)( grad_dev->imageData + y * grad_dev->widthStep );

		for ( int x = 0; x < area_roi.width; ++x, ++pLumImg, ++pGradImg, ++pLumMean, ++pLumDev, ++pGradMean, ++pGradDev )
		{
			*pLumMean += *pLumImg;					// accumulate sum
			*pLumDev += *pLumImg * *pLumImg;		// accumulate sum of squares

			*pGradMean += *pGradImg;				// accumulate sum
			*pGradDev += *pGradImg * *pGradImg;		// accumulate sum of squares
		}
	}

	return true;
}



bool AreaVarianceDetector::finalise_model()
{
	assert( ! model_initialised );

	// a minimum number of samples must have been learnt
	if ( num_samples < 5 )
	{
		trace_log << "At least 5 samples must be learnt before the model can be finalised!" << endl;
		return false;
	}
	assert( lum_mean && lum_dev && grad_mean && grad_dev );


	for ( int y = 0; y < area_roi.height; ++y )
	{
		double* pLumMean = (double*)( lum_mean->imageData + y * lum_mean->widthStep );
		double* pLumDev  = (double*)( lum_dev->imageData + y * lum_dev->widthStep );
		double* pGradMean = (double*)( grad_mean->imageData + y * grad_mean->widthStep );
		double* pGradDev  = (double*)( grad_dev->imageData + y * grad_dev->widthStep );

		for ( int x = 0; x < area_roi.width; ++x, ++pLumMean, ++pLumDev, ++pGradMean, ++pGradDev )
		{
			*pLumMean /= num_samples;
			*pLumDev = sqrt( *pLumDev / num_samples - *pLumMean * *pLumMean );

			*pGradMean /= num_samples;
			*pGradDev = sqrt( *pGradDev / num_samples - *pGradMean * *pGradMean );
		}
	}

	model_initialised = true;

	return true;
}



bool AreaVarianceDetector::process_image( IplImage* image )
{
	assert( image );
	assert( model_initialised );
	assert( area_roi.width <= image->width && 
			area_roi.height <= image->height );

	variation_score = 0.0;

	
	if ( debug )
	{
		assert( debug_image );
		CvRect old_rect = cvGetImageROI( image );
		cvSetImageROI( image, area_roi );
		cvCopy( image, debug_image );
		cvSetImageROI( image, old_rect );

		if ( debug_image2 != NULL )
			cvReleaseImage( &debug_image2 );
		debug_image2 = cvCloneImage( debug_image );
		assert( debug_image2 );
		cvSet( debug_image2, cvScalar( 0 ) );

		cvNamedWindow( "AreaVarianceDetector-debug", CV_WINDOW_AUTOSIZE );
		cvNamedWindow( "AreaVarianceDetector-debug2", CV_WINDOW_AUTOSIZE );
		cvNamedWindow( "AreaVarianceDetector-debug3", CV_WINDOW_AUTOSIZE );
		cvShowImage( "AreaVarianceDetector-debug", debug_image );
	}


	// generate gradient image
	assert( wrk1 && wrk2 && grad_image );	
	CvRect old_rect = cvGetImageROI( image );
	cvSetImageROI( image, area_roi );
	cvSmooth( image, wrk1, CV_GAUSSIAN, mask_size, mask_size );
	cvLaplace( wrk1, wrk2, mask_size );
	cvConvertScaleAbs( wrk2, grad_image );
	cvSetImageROI( image, old_rect );

	
	// find differences
	for ( int y = 0; y < area_roi.height; ++y )
	{
		unsigned char* pLumImg  = (unsigned char*)( image->imageData + ( y + area_roi.y ) * image->widthStep + area_roi.x );
		unsigned char* pGradImg = (unsigned char*)( grad_image->imageData + y * grad_image->widthStep );
		double* pLumMean  = (double*)( lum_mean->imageData + y * lum_mean->widthStep );
		double* pLumDev   = (double*)( lum_dev->imageData + y * lum_dev->widthStep );
		double* pGradMean = (double*)( grad_mean->imageData + y * grad_mean->widthStep );
		double* pGradDev  = (double*)( grad_dev->imageData + y * grad_dev->widthStep );
		unsigned char* pDiff = (unsigned char*)( diff->imageData + y * diff->widthStep );

		if ( debug )
		{
			unsigned char* pDbg2 = (unsigned char*)( debug_image2->imageData + y * debug_image2->widthStep );

			for ( int x = 0; x < area_roi.width; ++x, ++pLumImg, ++pGradImg, ++pLumMean, ++pLumDev, ++pGradMean, ++pGradDev, ++pDiff, ++pDbg2 )
			{
				double dl = fabs( *pLumImg - *pLumMean );
				double dg = fabs( *pGradImg - *pGradMean );
				if ( dl > num_sigmas * *pLumDev || dg > num_sigmas * *pGradDev )
					*pDiff = 255;
				else
					*pDiff = 0;
				*pDbg2 = *pDiff;
			}
		}
		else
		{
			for ( int x = 0; x < area_roi.width; ++x, ++pLumImg, ++pGradImg, ++pLumMean, ++pLumDev, ++pGradMean, ++pGradDev, ++pDiff )
			{
				double dl = fabs( *pLumImg - *pLumMean );
				double dg = fabs( *pGradImg - *pGradMean );
				if ( dl > num_sigmas * *pLumDev || dg > num_sigmas * *pGradDev )
					*pDiff = 255;
				else
					*pDiff = 0;
			}
		}
	}


	// do neighbourhood support to eliminate noisy detections
	bool changes = false;
	int loops = 10;
	do
	{
		for ( y = mask_size; y < area_roi.height - mask_size; ++y )
		{
			int x = mask_size;

			unsigned char* pDiff0 = (unsigned char*)( diff->imageData + (y-1) * diff->widthStep + x );
			unsigned char* pDiff1 = (unsigned char*)( diff->imageData + (y  ) * diff->widthStep + x );
			unsigned char* pDiff2 = (unsigned char*)( diff->imageData + (y+1) * diff->widthStep + x );

			for ( ; x < area_roi.width - mask_size; ++x, ++pDiff0, ++pDiff1, ++pDiff2 )
			{
				int n = ( *(pDiff0 - 1) == 255 ) + ( *(pDiff0) == 255 ) + ( *(pDiff0 + 1) == 255 ) +
						( *(pDiff1 - 1) == 255 ) +                        ( *(pDiff1 + 1) == 255 ) +
						( *(pDiff2 - 1) == 255 ) + ( *(pDiff2) == 255 ) + ( *(pDiff2 + 1) == 255 );

				if ( *pDiff1 == 255 && n < 4 )	// eliminate if less than 4 neighbours active
				{
					*pDiff1 = 0;
					changes = true;
				}

				if ( *pDiff1 == 0 && n > 5 )	// activate if more than 5 neighbours active
				{
					*pDiff1 = 255;
					changes = true;
				}
			}
		}
	}
	while( --loops && changes );


	// get measure of difference
	for ( y = mask_size; y < area_roi.height - mask_size; ++y )
	{
		int x = mask_size;

		unsigned char* pDiff = (unsigned char*)( diff->imageData + y * diff->widthStep + x );

		for ( ; x < area_roi.width - mask_size; ++x, ++pDiff )
		{
			if ( *pDiff )
				++variation_score;
		}
	}

	variation_score /= ( area_roi.width * area_roi.height );


	if ( debug )
	{
		cvShowImage( "AreaVarianceDetector-debug2", debug_image2 );
		cvShowImage( "AreaVarianceDetector-debug3", diff );
	}


	return true;
}



