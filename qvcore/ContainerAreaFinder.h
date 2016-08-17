
#ifndef __CONTAINER_AREA_FINDER_H__
#define __CONTAINER_AREA_FINDER_H__


#include "common.h"


///EXPIMP_TEMPLATE template class DLL_EXPORT std::vector< CvRect >;
///EXPIMP_TEMPLATE template class DLL_EXPORT std::vector< IplImage* >;
///EXPIMP_TEMPLATE template class DLL_EXPORT std::vector< CvPoint3D32f >;



class DLL_EXPORT ContainerAreaFinder
{
private:

	std::vector< CvRect >		template_roi;

	CvRect						search_roi;
	CvRect						label_roi;

	bool						model_initialised;
	std::vector< IplImage* >	label_templates;

	std::vector< CvPoint3D32f >	ref_eqn;

	bool						debug;
	IplImage*					debug_image;
	IplImage*					debug_image2;
	IplImage*					debug_image3;

	double						min_line_angle_threshold;	// in degrees

	double						rotation_correction;
	CvPoint3D32f				displacement_correction;

	// pre-allocated temporary objects
	std::vector< IplImage* >	wrk;
	std::vector< CvRect >		roi;	

public:


	ContainerAreaFinder();


	bool add_template_area( CvRect& roi );
	
	
	
	int num_templates() const
	{
		return template_roi.size();
	}



	void set_debug( bool debug = true )
	{
		this->debug = debug;
	}

	
	
	void set_label_area( const CvRect& area )
	{
		assert( ! model_initialised );
		label_roi = area;
	}



	CvRect get_label_area() const
	{
		return label_roi;
	}

	
	
	void set_search_area( const CvRect& area )
	{
		assert( ! model_initialised );
		search_roi = area;
	}



	CvRect get_search_area() const
	{
		return search_roi;
	}



	virtual ~ContainerAreaFinder();



	double get_rotation_correction( bool in_degrees = true ) const
	{
		return in_degrees ? rotation_correction * 180 / CV_PI : rotation_correction;
	}



	CvPoint2D32f get_displacement_correction() const
	{
		return cvPoint2D32f( displacement_correction.x / displacement_correction.z, 
							 displacement_correction.y / displacement_correction.z );
	}



	void set_min_line_angle_threshold( double line_angle_threshold )
	{
		assert( line_angle_threshold > 0.0 && line_angle_threshold < 90.0 );
		min_line_angle_threshold = line_angle_threshold;
	}



	double get_min_line_angle_threshold() const
	{
		return min_line_angle_threshold;
	}



	bool finalise_model( IplImage* image );


	bool process_image( IplImage* image, CvMat* registration_matrix );

};




#endif

