// Base interface class for communication between NeuRA2 GUI and image processing operators
// by Daniel Jungblut, G-CSC, University of Frankfurt, July 2009


#ifndef _CIMAGEPROP_H_
#define _CIMAGEPROP_H_

#include <stddef.h>

#include "LogFunction.h"

// Forward Declaration:
class C3DImage;
class CSubimage;
class CMesh;
namespace neura2 {

class CImagePrOp {
	
public:
	
	// Constructor and Destructor:
	CImagePrOp () {}
	virtual ~CImagePrOp() {}
	
	virtual int getSubimageSize(int & size_x,
															 int & size_y,
															 int & size_z,
															 int & overlap_x,
															 int & overlap_y,
															 int & overlap_z,
															 int & interpolate_x,
															 int & interpolate_y,
															 int & interpolate_z,
															 const size_t) {
		size_x = size_x;
		size_y = size_y;
		size_z = size_z;
		overlap_x = 0;
		overlap_y = 0;
		overlap_z = 0;
		interpolate_x = 0;
		interpolate_y = 0;
		interpolate_z = 0;
		
		return 0;
	}
	
	virtual int process(CSubimage & subimage,
											 C3DImage* &image, 
											 const int nArgInts,
											 int * argInts,
											 const int nArgFloats,
											 float * argFloats,
											 const int nArgStrings,
											 char ** argStrings) = 0;
	
	virtual int getSubimageSizePreview(int &, int &, int &, const size_t) { return 1; }
	
	virtual int processPreview(CSubimage &, const int, int *, const int, float *, const int, char **) { return 1; }															 
	
  virtual int log(const int,
									int *,
									const int,
									float *,
									const int,
									char **) {
		return 1;
	}

	
};

}

#endif 

