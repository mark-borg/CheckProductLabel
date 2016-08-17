
#ifndef __HIGH_TEXTURED_AREA_FINDER_H__
#define __HIGH_TEXTURED_AREA_FINDER_H__


#include "common.h"



class DLL_EXPORT HighTexturedAreaFinder
{
private:


	int			template_width, template_height;
	CvRect		search_roi;
	int			min_template_distance;
	double		min_area_strength;

	IplImage*	eigen_map;

	bool		debug;
	IplImage*	debug_image;


public:


	HighTexturedAreaFinder();


	void set_debug( bool debug = true )
	{
		this->debug = debug;
	}

		
	bool set_template_width( int width );


	int get_template_width() const
	{
		return template_width;
	}


	bool set_template_height( int height );


	int get_template_height() const
	{
		return template_height;
	}


	void set_min_area_strength( double strength )
	{
		min_area_strength = strength;
	}


	double get_min_area_strength() const
	{
		return min_area_strength;
	}



	void set_min_template_distance( int distance )
	{
		min_template_distance = distance;
	}



	int get_min_template_distance() const
	{
		return min_template_distance;
	}



	void set_search_area( const CvRect& area )
	{
		search_roi = area;
	}



	CvRect get_search_area() const
	{
		return search_roi;
	}



	virtual ~HighTexturedAreaFinder();



	bool process_image( IplImage* image );


	bool find_strong_textured_area( CvRect& area );

};




#endif

