
#ifndef __AREA_VARIANCE_DETECTOR_H__
#define __AREA_VARIANCE_DETECTOR_H__


#include "common.h"



class DLL_EXPORT AreaVarianceDetector
{
private:

	CvRect				area_roi;

	double				num_sigmas;
	int					mask_size;

	bool				model_initialised;
	int					num_samples;
	IplImage*			lum_mean;
	IplImage*			lum_dev;
	IplImage*			grad_mean;
	IplImage*			grad_dev;
	IplImage*			diff;

	double				variation_score;

	bool				debug;
	IplImage*			debug_image;
	IplImage*			debug_image2;

	// temporary image buffers
	IplImage*			wrk1;
	IplImage*			wrk2;
	IplImage*			grad_image;

public:


	AreaVarianceDetector();


	virtual ~AreaVarianceDetector();


	void set_num_sigmas( double num_sigmas )
	{
		assert( num_sigmas > 0.0 );
		this->num_sigmas = num_sigmas;
	}



	double get_num_sigmas() const
	{
		return num_sigmas;
	}



	void set_mask_size( int s )
	{
		assert( s > 1 );
		assert( s % 2 == 0 );
		mask_size = s;
	}



	int get_mask_size() const
	{
		return mask_size;
	}



	void set_debug( bool debug = true )
	{
		this->debug = debug;
	}

	
	
	void set_area( const CvRect& area )
	{
		assert( ! model_initialised );
		area_roi = area;
	}



	CvRect get_area() const
	{
		return area_roi;
	}



	bool learn_sample( IplImage* image );
	

	bool finalise_model();


	bool process_image( IplImage* image );



	double get_variation_score() const
	{
		assert( model_initialised );

		return variation_score;
	}



	const IplImage* get_variation_mask() const
	{
		assert( model_initialised );

		return diff;
	}

};


#endif

