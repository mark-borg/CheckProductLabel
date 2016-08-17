
#ifndef __CONTAINER_OUTLINE_FINDER_H__
#define __CONTAINER_OUTLINE_FINDER_H__


#include "common.h"


///EXPIMP_TEMPLATE template class DLL_EXPORT std::vector< CvRect >;
///EXPIMP_TEMPLATE template class DLL_EXPORT std::vector< CvPoint3D32f >;
///EXPIMP_TEMPLATE template class DLL_EXPORT std::vector< bool >;
///EXPIMP_TEMPLATE template class DLL_EXPORT std::vector< double >;



class DLL_EXPORT ContainerOutlineFinder
{
private:

	std::vector< CvRect >		regions;

	std::vector< bool >			line_valid;
	std::vector< CvPoint3D32f >	line_eqn;
	std::vector< CvPoint3D32f >	line_centrept;
	std::vector< double >		line_correlation;
	std::vector< CvPoint3D32f >	line_intersect;

	bool						model_initialised;
	std::vector< bool >			ref_valid;
	std::vector< CvPoint3D32f >	ref_eqn;
	std::vector< CvPoint3D32f >	ref_centrept;
	std::vector< double >		ref_correlation;
	std::vector< CvPoint3D32f >	ref_intersect;

	bool						debug;
	IplImage*					debug_image;
	IplImage*					debug_image2;

	double						min_line_fit_threshold;
	double						min_line_angle_threshold;	// in degrees
	unsigned char				contour_map_threshold;

	double						rotation_correction;
	CvPoint3D32f				displacement_correction;
	

public:


	ContainerOutlineFinder();

	
	~ContainerOutlineFinder();

	
	bool add_model_region( IplImage* contour_map, CvRect& roi );


	bool add_data_region( IplImage* contour_map, int region_index );


	bool finalise_model();


	double get_rotation_correction( bool in_degrees = true ) const
	{
		return in_degrees ? rotation_correction * 180 / CV_PI : rotation_correction;
	}



	CvPoint2D32f get_displacement_correction() const
	{
		return cvPoint2D32f( displacement_correction.x / displacement_correction.z, 
							 displacement_correction.y / displacement_correction.z );
	}



	void set_debug( bool debug = true )
	{
		this->debug = debug;
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



	void set_line_fit_threshold( double line_fit_threshold )
	{
		assert( line_fit_threshold > 0.0 && line_fit_threshold <= 1.0 );
		min_line_fit_threshold = line_fit_threshold;
	}



	double get_line_fit_threshold() const
	{
		return min_line_fit_threshold;
	}



	void set_contour_map_threshold( unsigned char contour_map_threshold )
	{
		this->contour_map_threshold = contour_map_threshold;
	}



	unsigned char get_contour_map_threshold() const
	{
		return contour_map_threshold;
	}

	
	bool process_image( IplImage* contour_map, CvMat* registration_matrix );



private:

	
	bool fit_line_to_region( IplImage* contour_map, int n );


	bool find_line_intersections();


};



#endif

