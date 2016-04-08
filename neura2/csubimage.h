/*
 *  CSubimage.h
 *  
 *
 *  Created by Daniel on 21.11.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#ifndef _CSUBIMAGE_H_
#define _CSUBIMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include "../../TiffImage/neura2/constants.h"
namespace neura2 {

class CSubimage {
	
private:
	
	int size_x;
	int size_y;
	int size_z;
	
	int offset_x;
	int offset_y;
	int offset_z;
	
	int overlap_x;
	int overlap_y;
	int overlap_z;
	
	uchar * p_data;

	
	// Instanzen vom Typ CFilterCube sollen nicht kopiert werden (siehe Effective C++, Item 6, 3rd Edition)
	CSubimage (const CSubimage&);
	CSubimage& operator=(const CSubimage&);
	
public:
	
	CSubimage()
	: size_x(0), size_y(0), size_z(0),
	  offset_x(0), offset_y(0), offset_z(0),
	  overlap_x(0),	overlap_y(0), overlap_z(0), p_data(0)
	{}
	
	~CSubimage() {
		if(p_data != 0) delete [] p_data;
	}
	
	void set_size_x(const int size_x) { this->size_x = size_x; }
	void set_size_y(const int size_y) { this->size_y = size_y; }
	void set_size_z(const int size_z) { this->size_z = size_z; }
	void set_offset_x(const int offset_x) { this->offset_x = offset_x; }
	void set_offset_y(const int offset_y) { this->offset_y = offset_y; }
	void set_offset_z(const int offset_z) { this->offset_z = offset_z; }
	void set_overlap_x(const int overlap_x) { this->overlap_x = overlap_x; }
	void set_overlap_y(const int overlap_y) { this->overlap_y = overlap_y; }
	void set_overlap_z(const int overlap_z) { this->overlap_z = overlap_z; }
	
	int get_size_x() { return this->size_x; }
	int get_size_y() { return this->size_y; }
	int get_size_z() { return this->size_z; }
	int get_offset_x() { return this->offset_x; }
	int get_offset_y() { return this->offset_y; }
	int get_offset_z() { return this->offset_z; }
	int get_overlap_x() { return this->overlap_x; }
	int get_overlap_y() { return this->overlap_y; }
	int get_overlap_z() { return this->overlap_z; }
	
	int get_total_size_x() { return this->size_x + this->overlap_x; }
	int get_total_size_y() { return this->size_y + this->overlap_y; }
	int get_total_size_z() { return this->size_z + this->overlap_z; }
	
	int allocate() {
		if(p_data != 0) delete [] p_data;
		size_t size = (size_x + overlap_x) * (size_y + overlap_y) * (size_z + overlap_z);
		p_data = new uchar[size];
		if(p_data == 0) {
			printf("Problems while allocating p_data in CFilterCube::allocate.\n");
			return 1;
		}
		return 0;
	}
		
	uchar * get_pointer_to_data() {
		return p_data;
	}
		
	uchar get(const int xx, const int yy, const int zz) {
		if((p_data != 0) && (xx >= 0) && (xx < get_total_size_x()) && (yy >= 0) && (yy < get_total_size_y())
			 && (zz >= 0) && (zz < get_total_size_z()))
			 return p_data[zz * get_total_size_x() * get_total_size_y() + yy * get_total_size_x() + xx];
					 
		return uchar(0);
	}
	
	void set(const int xx, const int yy, const int zz, const uchar value) {
	  if((p_data != 0) && (xx >= 0) && (xx < get_total_size_x()) && (yy >= 0) && (yy < get_total_size_y())
			 && (zz >= 0) && (zz < get_total_size_z()))

		  p_data[zz * get_total_size_x() * get_total_size_y() + yy * get_total_size_x() + xx] = value;
		
	}
		
			
	
};
}

#endif


