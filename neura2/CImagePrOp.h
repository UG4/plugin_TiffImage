/*
 * Copyright (c) 2011 - 2017 :  G-CSC, Goethe University Frankfurt
 * Author: Daniel Jungblut
 *
 * This file is part of UG4.
 *
 * UG4 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 3 (as published by the
 * Free Software Foundation) with the following additional attribution
 * requirements (according to LGPL/GPL v3 §7):
 *
 * (1) The following notice must be displayed in the Appropriate Legal Notices
 * of covered and combined works: "Based on UG4 (www.ug4.org/license)".
 *
 * (2) The following notice must be displayed at a prominent place in the
 * terminal output of covered works: "Based on UG4 (www.ug4.org/license)".
 *
 * (3) The following bibliography is recommended for citation and must be
 * preserved in all covered files:
 * "Reiter, S., Vogel, A., Heppner, I., Rupp, M., and Wittum, G. A massively
 *   parallel geometric multigrid solver on hierarchically distributed grids.
 *   Computing and visualization in science 16, 4 (2013), 151-164"
 * "Vogel, A., Reiter, S., Rupp, M., Nägel, A., and Wittum, G. UG4 -- a novel
 *   flexible software system for simulating pde based models on high performance
 *   computers. Computing and visualization in science 16, 4 (2013), 165-179"
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 */

#ifndef _NEURA2__CIMAGEPROP_H_
#define _NEURA2__CIMAGEPROP_H_

#include <stddef.h>


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

