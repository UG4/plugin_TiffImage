/*
 *  c3dimage.h
 *  
 *
 *  Created by Daniel on 20.11.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _NEURA2__C3DIMAGE_H_
#define _NEURA2__C3DIMAGE_H_

#include <iostream>

#include "cimage.h"
#include "constants.h"
#include "csubimage.h"

namespace neura2 {
class C3DImage {
	
private:
	
	int size_x;        // Dimension des Bildes in x-Richtung
	int size_y;        // Dimension des Bildes in y-Richtung
	int size_z;        // Dimension des Bildes in z-Richtung
	
	CImage * p_images;   // Array, in dem die Einzelbilder gespeichert sind

	

	// Instanzen vom Typ C3DImage dürfen nicht kopiert werden, daher werden Copy- und
	// Copy-Assignment Konstruktor private deklariert, aber nicht implementiert:
	
	C3DImage& operator=(const C3DImage&);
	
	
public:
	
	// Konstruktor initialisiert Werte:
	C3DImage()
	: size_x(0), size_y(0), size_z(0), p_images(NULL)
	{}
	
	
	C3DImage(const int asize_x, const int asize_y, const int asize_z)
	: size_x(asize_x), size_y(asize_y), size_z(asize_z)
	{
		
		p_images = new CImage[size_z];
		
		for(int i = 0; i < size_z; i++)
			p_images[i].setSize(size_x, size_y);
		
	}
	
	
	// Destruktor gibt allokierten Speicherbereich wieder frei:
	~C3DImage() { 
	  if(p_images) delete [] p_images;
	}
	
	// Funktionen zum lesen und schreiben verschiedener Dateitypen:
	int read_dat(const char * filename);
	int write_dat(const char * filename);
	
	int read_raw(const char * filename, const int size_x, const int size_y, const int size_z);
	int write_raw(const char * filename);
	  
#ifdef NEURA2_WITH_TIFF_
	int read_tiff(const char * filename);
	int write_tiff(const char * filename);
#endif
	
	// Gibt Dimensionen des Bildes zurück:
	int get_size_x() const { return this->size_x; }
	int get_size_y() const { return this->size_y; }
	int get_size_z() const { return this->size_z; }
	
	
	// Gibt Grauwert des Bildes an der Stelle i,j,k zurück:
	uchar get(const int i, const int j, const int k) const {
		if((p_images != NULL) && (k >= 0) && (k < size_z))
			return p_images[k].get(i, j);
		return uchar(0);		
	}
	
	
	// Schreibt den Grauwert value an die Stelle i,j,k des Bildes:
	void set(const int i, const int j, const int k, const int value) {
		if((p_images != NULL) && (k >= 0) && (k < size_z))
			p_images[k].set(i, j, value);
	}	
	
	
	// Gibt Zeiger auf die Daten des k-ten Bildes zurück:
	uchar * get_pointer_to_image_data(const int k) {
	  if((p_images != 0) && (k >= 0) && (k < size_z))
			return p_images[k].get_pointer_to_data();
		return NULL;
	}
	
	
	// Erzeugt Subcube, der entsprechenden Dimension mit Datentyp unsigned char:
	int create_subimage(CSubimage & subimage,
											const int size_x, const int size_y, const int size_z,
											const int offset_x, const int offset_y, const int offset_z,
											const int overlap_x, const int overlap_y, const int overlap_z);
	
	
	// Schreibt das Subimage mit Subimagekoordinaten xx, yy, zz zurück:
	int writeback_subimage(CSubimage & subimage, 
												 const int xx,
												 const int yy, 
												 const int zz,
												 const int number_of_subcubes_x,
												 const int number_of_subcubes_y,
												 const int number_of_subcubes_z,
												 bool * subimage_written);
	
	
	int normalize() {
	
		uchar max = 0;
		for(int k = 0; k < size_z; k++)
			for(int j = 0; j < size_y; j++)
				for(int i = 0; i < size_x; i++)
					if(get(i, j, k) > max) max = get(i, j, k);
		
		if(max > 0) {
  		uchar LUT[256];
		
	  	for(int i = 0; i < 256; i++)
		  	LUT[i] = uchar(float(i) / float(max) * 255.f);
			
			for(int k = 0; k < size_z; k++) 
				for(int j = 0; j < size_y; j++)
					for(int i = 0; i < size_x; i++)
						set(i, j, k, LUT[get(i, j, k)]);
		
		}
		return 0;									 
	}

	C3DImage(const C3DImage * image) {
		
		size_x = image->size_x;
		size_y = image->size_y;
		size_z = image->size_z;
		
		if(size_z > 0) {
  		p_images = new CImage[size_z];
		
	  	for(int i = 0; i < size_z; i++)
		  	p_images[i].set_all(image->p_images[i].get_pointer_to_data(), size_x, size_y);
			
		}
		
	}
	
        int writePPM(const char * filename, const int z);

        int write_binvox(const char * filename);

	
};


}
#endif


