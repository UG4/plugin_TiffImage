/*
 *  c3dimage.cpp
 *  
 *
 *  Created by Daniel on 20.11.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _C3DIMAGE_CPP_
#define _C3DIMAGE_CPP_


#include "../../TiffImage/neura2/c3dimage.h"

namespace neura2 {
int C3DImage::read_dat(const char * filename) {
	
  FILE * file;
	
	if((file = fopen(filename, "rb")) == NULL) {
	  std::cout << "File " << filename << " not found ... exiting." << std::endl;
		return 1;
  }
	
	fread(&size_x, 1, 4, file);
  fread(&size_y, 1, 4, file);
  fread(&size_z, 1, 4, file);
  
	std::cout << "File " << filename << " has dimensions " << size_x << " x " << size_y << " x " << size_z << std::endl;
	
  fseek(file, 12, SEEK_CUR);
	
	p_images = new CImage[size_z];

	
	for(int zz = 0; zz < size_z; zz++) 
		p_images[zz].read(file, size_x, size_y);
	
	fclose(file);
	
	return 0;
	
}


int C3DImage::write_dat(const char * filename) {
	
  FILE * file;
  
	if((file = fopen(filename, "wb")) == NULL) {
		std::cout << "File " << filename << " not found ... exiting." << std::endl;
		return 1;
	}
	
	fwrite(&size_x, 4, 1, file);
	fwrite(&size_y, 4, 1, file);
	fwrite(&size_z, 4, 1, file);
	
	int dx = 1;
  int dy = 1;
  int dz = 1;
  
  fwrite(&dx, 4, 1, file);
  fwrite(&dy, 4, 1, file);
  fwrite(&dz, 4, 1, file);
	
  for(int zz = 0; zz < size_z; zz++) 
		p_images[zz].write(file);
	
	fclose(file);
	
	std::cout << filename << " written successfully." << std::endl;
	
	return 0;
}


int C3DImage::read_raw(const char * filename, const int size_x, const int size_y, const int size_z) {
											 
  FILE * file;
											 
	if((file = fopen(filename, "rb")) == NULL) {
	  std::cout << "File " << filename << " not found ... exiting." << std::endl;
		return 1;
  }
											 
  this->size_x = size_x;
	this->size_y = size_y;
	this->size_z = size_z;
  
	std::cout << "File " << filename << " has dimensions " << size_x << " x " << size_y << " x " << size_z << std::endl;
	
	p_images = new CImage[size_z];

	for(int zz = 0; zz < size_z; zz++) 
		p_images[zz].read(file, size_x, size_y);
	
	fclose(file);

	return 0;

}


int C3DImage::write_raw(const char * filename) {
	
  FILE * file;
  
	if((file = fopen(filename, "wb")) == NULL) {
		std::cout << "File " << filename << " not found ... exiting." << std::endl;
		return 1;
	}
		
  for(int zz = 0; zz < size_z; zz++) 
		p_images[zz].write(file);
	
	fclose(file);
	
	std::cout << filename << " written successfully." << std::endl;
	
	return 0;
}




// Erzeugt Subcube, der entsprechenden Dimension mit Datentyp unsigned char:
int C3DImage::create_subimage(CSubimage & subimage,
															const int size_x, const int size_y, const int size_z,
																		const int offset_x, const int offset_y, const int offset_z,
																		const int overlap_x, const int overlap_y, const int overlap_z) {
	
	
	// Bestimmen von size_x, offset_x und overlap_x:
	int overlap_x_2 = overlap_x;
	
	if(offset_x + size_x + overlap_x > this->size_x)
		overlap_x_2 = 0;
	
	subimage.set_size_x(size_x);
	subimage.set_offset_x(offset_x);
	subimage.set_overlap_x(overlap_x_2);
	
	
	// Bestimmen von size_y, offset_y und overlap_y:
	int overlap_y_2 = overlap_y;
	
	if(offset_y + size_y + overlap_y > this->size_y)
		overlap_y_2 = 0;
	
	subimage.set_size_y(size_y);
	subimage.set_offset_y(offset_y);
	subimage.set_overlap_y(overlap_y_2);
	
	
	// Bestimmen von size_z, offset_z und overlap_z:
	int overlap_z_2 = overlap_z;
	if(offset_z + size_z + overlap_z > this->size_z)
		overlap_z_2 = 0;
	
	int size_z_2 = size_z;
	
	if(overlap_z_2 == 0)
		while ((offset_z + size_z_2) > this->size_z) size_z_2 --;
	
	if(size_z_2 != 1)
    while((size_z_2 % 4) != 0) size_z_2 ++;
	
	subimage.set_size_z(size_z_2);
	subimage.set_offset_z(offset_z);
	subimage.set_overlap_z(overlap_z_2);
	
	//std::cout << "Creating subcube with dimensions " << size_x << " x " << size_y << " x " << size_z << std::endl;
	//std::cout << "At offset: " << offset_x << ", " << offset_y << ", " << offset_z << std::endl;
	//std::cout << "And overlap: " << overlap_x_2 << ", " << overlap_y_2 << ", " << overlap_z_2 << std::endl << std::endl;
	
	
	// Allokieren von Speicher für den FilterCube:
	subimage.allocate();
	
	// Zeiger auf den allokierten Speicher beschaffen:
	uchar * subimage_data = subimage.get_pointer_to_data();
	
	
	// Kopieren der Daten in den Filtercube:
	for(int k = offset_z; k < offset_z + size_z_2 + overlap_z_2; k++) {
		
		uchar * p_data = get_pointer_to_image_data(k);
		
		for(int j = offset_y; j < offset_y + size_y + overlap_y_2; j++) {
			
			if(p_data != NULL)
  			memcpy(&subimage_data[(k - offset_z) * (size_x + overlap_x_2) * (size_y + overlap_y_2)
											+ (j - offset_y) * (size_x + overlap_x_2)], &p_data[j * this->size_x + offset_x], size_x + overlap_x_2);
			
			else memset(&subimage_data[(k - offset_z) * (size_x + overlap_x_2) * (size_y + overlap_y_2)
													 + (j - offset_y) * (size_x + overlap_x_2)], '\0', size_x + overlap_x_2);

		}
	}
	
  return 0;	
	
}




static inline int get_subcube_index(const int xx,
														 const int yy, 
														 const int zz, 
														 const int number_of_subcubes_x,
														 const int number_of_subcubes_y) {
	
	return zz * (number_of_subcubes_x * number_of_subcubes_y) + yy * number_of_subcubes_x + xx;
}


int C3DImage::writeback_subimage(CSubimage & subimage, 
																 const int xx,
																 const int yy,
																 const int zz,
																 const int number_of_subcubes_x,
																 const int number_of_subcubes_y,
																 const int number_of_subcubes_z,
                                 bool * subimage_written) {
	
	int offset_x = subimage.get_offset_x();
	int offset_y = subimage.get_offset_y();
	int offset_z = subimage.get_offset_z();
	
	//std::cout << offset_x << " " << offset_y << " " << offset_z << std::endl;
	
	const int overlap_const = 6;
  int i_min = offset_x + 16 - overlap_const; if(xx == 0) i_min = 0;
	int i_max = offset_x + subimage.get_size_x(); if(xx < (number_of_subcubes_x - 1)) i_max += overlap_const;
	int j_min = offset_y + 16 - overlap_const; if(yy == 0) j_min = 0;
	int j_max = offset_y + subimage.get_size_y(); if(yy < (number_of_subcubes_y - 1)) j_max += overlap_const;
	int k_min = offset_z + 16 - overlap_const; if(zz == 0) k_min = 0;
	int k_max = offset_z + subimage.get_size_z(); if(zz < (number_of_subcubes_z - 1)) k_max += overlap_const;
	
	//std::cout << i_min << " " << i_max << " " << j_min << " " << j_max << " " << k_min << " " << k_max << std::endl;

	/*
	if(subimage.get_overlap_x() < 16) {
	  i_min = offset_x;
		i_max = offset_x + subimage.get_size_x();
	}
	
	if(subimage.get_overlap_y() < 16) {
	  j_min = offset_y;
		j_max = offset_y + subimage.get_size_y();
	}
	
	if(subimage.get_overlap_z() < 16) {
		k_min = offset_z;
		k_max = offset_z + subimage.get_size_z();
	}*/
	
	
	if(subimage.get_size_z() == 1) {
	  k_min = offset_z;
		k_max = offset_z;
	}
	// */
	
	uchar * data_in = subimage.get_pointer_to_data();
	
	for(int k = k_min; k < k_max; k++) {
		
		uchar * data_out = get_pointer_to_image_data(k);
		
		for(int j = j_min; j < j_max; j++) {
			
			if(data_out != NULL) 
				memcpy(&data_out[j * get_size_x() + i_min], 
							 &data_in[(k-offset_z) * subimage.get_total_size_x() * subimage.get_total_size_y() + (j-offset_y) * subimage.get_total_size_x() + i_min - offset_x], 
							 i_max - i_min);
			
			/* for(int i = i_min; i < i_max; i++) {
			 
			 uchar uc_temp = subimage.get((i-offset_x), (j-offset_y), (k-offset_z));
			 
			 set(i, j, k, uc_temp);  	
			 }  // */
		}
	}
	
	
	
  // Unterer Rand in x-Richtung:
	if(xx > 0) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx-1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)];
		
		
		for(int k = k_min; k < k_max; k++)
			for(int j = j_min; j < j_max; j++)
				for(int i = i_min - 4; i < i_min; i++) {
					
					float factor = (float(i - i_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					//if(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)) > 0) std::cout << subimage.get((i-offset_x), (j-offset_y), (k-offset_z)) << std::endl;
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					
					set(i, j, k, uc_temp);	
					
				}
	}
	
	// Oberer Rand in x-Richtung:
	if(xx < (number_of_subcubes_x - 1)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx+1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)];
	  
		for(int k = k_min; k < k_max; k++)
			for(int j = j_min; j < j_max; j++)
				for(int i = i_max; i < i_max + 4; i++) {
					
					float factor = 1.f - (float(i - i_max) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);	
					
				}
		
	}
	
	// Unterer Rand in y-Richtung:
	if(yy > 0) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min; k < k_max; k++)
			for(int j = j_min-4; j < j_min; j++)
				for(int i = i_min; i < i_max; i++) {
					
 					float factor = (float(j - j_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);	
					
				}
	}
	
	// Oberer Rand in x-Richtung:
	if(yy < (number_of_subcubes_y - 1)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)];
	  
		for(int k = k_min; k < k_max; k++)
			for(int j = j_max; j < j_max + 4; j++)
				for(int i = i_min; i < i_max; i++) {
					
					float factor = 1.f - (float(j - j_max) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);	
					
				}
		
	}
	
	// Unterer Rand in z-Richtung:
	if(zz > 0) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)];
	  
		for(int k = k_min-4; k < k_min; k++)
			for(int j = j_min; j < j_max; j++)
				for(int i = i_min; i < i_max; i++) {
					
 					float factor = (float(k - k_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);	
					
					
				}
		
	}
	
	// Oberer Rand in z-Richtung:
	if(zz < (number_of_subcubes_z - 1)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)];
	  
		for(int k = k_max; k < k_max + 4; k++)
			for(int j = j_min; j < j_max; j++)
				for(int i = i_min; i < i_max; i++) {
					
					float factor = 1.f - (float(k - k_max) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);	
					
				}
		
	}
	
	
	// INTERPOLATIONSKANTEN X-Y:
	if((xx > 0) && (yy > 0)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx-1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min; k < k_max; k++)
			for(int j = j_min - 4; j < j_min; j++)
				for(int i = i_min - 4; i < i_min; i++) {
					
					float factor = (float(i - i_min + 4) + 1.f) / 5.f * (float(j - j_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx > 0) && (yy < (number_of_subcubes_y - 1))) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx-1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min; k < k_max; k++)
			for(int j = j_max; j < j_max + 4; j++)
				for(int i = i_min - 4; i < i_min; i++) {
					
					float factor = (float(i - i_min + 4) + 1.f) / 5.f * (1.f - (float(j - j_max) + 1.f) / 5.f);
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx < (number_of_subcubes_x - 1)) && (yy > 0)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx+1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min; k < k_max; k++)
			for(int j = j_min - 4; j < j_min; j++)
				for(int i = i_max; i < i_max + 4; i++) {
					
					float factor = (1.f - (float(i - i_max) + 1.f) / 5.f) * (float(j - j_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx < (number_of_subcubes_x - 1)) && (yy < (number_of_subcubes_y - 1))) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx+1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min; k < k_max; k++)
			for(int j = j_max; j < j_max + 4; j++)
				for(int i = i_max; i < i_max + 4; i++) {
					
					float factor = (1.f - (float(i - i_max) + 1.f) / 5.f) * (1.f - (float(j - j_max) + 1.f) / 5.f);
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	// INTERPOLATIONSKANTEN X-Z:
	if((xx > 0) && (zz > 0)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx-1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min - 4; k < k_min; k++)
			for(int j = j_min; j < j_max; j++)
				for(int i = i_min - 4; i < i_min; i++) {
					
					float factor = (float(i - i_min + 4) + 1.f) / 5.f * (float(k - k_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx > 0) && (zz < (number_of_subcubes_z - 1))) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx-1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_max; k < k_max + 4; k++)
			for(int j = j_min; j < j_max; j++)
				for(int i = i_min - 4; i < i_min; i++) {
					
					float factor = (float(i - i_min + 4) + 1.f) / 5.f * (1.f - (float(k - k_max) + 1.f) / 5.f);
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx < (number_of_subcubes_x - 1)) && (zz > 0)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx+1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min - 4; k < k_min; k++)
			for(int j = j_min; j < j_max; j++)
				for(int i = i_max; i < i_max + 4; i++) {
					
					float factor = (1.f - (float(i - i_max) + 1.f) / 5.f) * (float(k - k_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx < (number_of_subcubes_x - 1)) && (zz < (number_of_subcubes_z - 1))) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx+1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_max; k < k_max + 4; k++)
			for(int j = j_min; j < j_max; j++)
				for(int i = i_max; i < i_max + 4; i++) {
					
					float factor = (1.f - (float(i - i_max) + 1.f) / 5.f) * (1.f - (float(k - k_max) + 1.f) / 5.f);
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	// INTERPOLATIONSKANTEN Y-Z:
	if((yy > 0) && (zz > 0)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz-1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min - 4; k < k_min; k++)
			for(int j = j_min - 4; j < j_min; j++)
				for(int i = i_min; i < i_max; i++) {
					
					float factor = (float(j - j_min + 4) + 1.f) / 5.f * (float(k - k_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((yy > 0) && (zz < (number_of_subcubes_z - 1))) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz+1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_max; k < k_max + 4; k++)
			for(int j = j_min - 4; j < j_min; j++)
				for(int i = i_min; i < i_max; i++) {
					
					float factor = (float(j - j_min + 4) + 1.f) / 5.f * (1.f - (float(k - k_max) + 1.f) / 5.f);
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((yy < (number_of_subcubes_y - 1)) && (zz > 0)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz-1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min - 4; k < k_min; k++)
			for(int j = j_max; j < j_max + 4; j++)
				for(int i = i_min; i < i_max; i++) {
					
					float factor = (1.f - (float(j - j_max) + 1.f) / 5.f) * (float(k - k_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((yy < (number_of_subcubes_y - 1)) && (zz < (number_of_subcubes_z - 1))) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz+1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_max; k < k_max + 4; k++)
			for(int j = j_max; j < j_max + 4; j++)
				for(int i = i_min; i < i_max; i++) {
					
					float factor = (1.f - (float(j - j_max) + 1.f) / 5.f) * (1.f - (float(k - k_max) + 1.f) / 5.f);
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	// INTERPOLATIONSECKEN:
  if((xx > 0) && (yy > 0) && (zz > 0)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx-1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy-1, zz-1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min - 4; k < k_min; k++)
			for(int j = j_min - 4; j < j_min; j++)
				for(int i = i_min - 4; i < i_min; i++) {
					
					float factor = (float(i - i_min + 4) + 1.f) / 5.f * (float(j - j_min + 4) + 1.f) / 5.f 
					* (float(k - k_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx > 0) && (yy > 0) && (zz < (number_of_subcubes_z - 1))) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx-1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy-1, zz+1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_max; k < k_max + 4; k++)
			for(int j = j_min - 4; j < j_min; j++)
				for(int i = i_min - 4; i < i_min; i++) {
					
					float factor = (float(i - i_min + 4) + 1.f) / 5.f * (float(j - j_min + 4) + 1.f) / 5.f 
					* (1.f - (float(k - k_max) + 1.f) / 5.f);
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx > 0) && (yy < (number_of_subcubes_y - 1)) && (zz > 0)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx-1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy+1, zz-1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min - 4; k < k_min; k++)
			for(int j = j_max; j < j_max + 4; j++)
				for(int i = i_min - 4; i < i_min; i++) {
					
					float factor = (float(i - i_min + 4) + 1.f) / 5.f * (1.f - (float(j - j_max) + 1.f) / 5.f) 
					* (float(k - k_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx < (number_of_subcubes_x - 1)) && (yy > 0) && (zz > 0)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx+1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy-1, zz-1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min - 4; k < k_min; k++)
			for(int j = j_min - 4; j < j_min; j++)
				for(int i = i_max; i < i_max + 4; i++) {
					
					float factor = (1.f - (float(i - i_max) + 1.f) / 5.f) * (float(j - j_min + 4) + 1.f) / 5.f 
					* (float(k - k_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx > 0) && (yy < (number_of_subcubes_y - 1)) && (zz < (number_of_subcubes_z - 1))) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx-1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx-1, yy+1, zz+1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_max; k < k_max + 4; k++)
			for(int j = j_max; j < j_max + 4; j++)
				for(int i = i_min - 4; i < i_min; i++) {
					
					float factor = (float(i - i_min + 4) + 1.f) / 5.f * (1.f - (float(j - j_max) + 1.f) / 5.f) 
					* (1.f - (float(k - k_max) + 1.f) / 5.f);
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx < (number_of_subcubes_x - 1)) && (yy > 0) && (zz < (number_of_subcubes_z - 1))) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx+1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy-1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy-1, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy-1, zz+1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_max; k < k_max + 4; k++)
			for(int j = j_min - 4; j < j_min; j++)
				for(int i = i_max; i < i_max + 4; i++) {
					
					float factor = (1.f - (float(i - i_max) + 1.f) / 5.f) * (float(j - j_min + 4) + 1.f) / 5.f 
					* (1.f - (float(k - k_max) + 1.f) / 5.f);
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	if((xx < (number_of_subcubes_x - 1)) && (yy < (number_of_subcubes_y - 1)) && (zz > 0)) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx+1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz-1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy+1, zz-1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_min - 4; k < k_min; k++)
			for(int j = j_max; j < j_max + 4; j++)
				for(int i = i_max; i < i_max + 4; i++) {
					
					float factor = (1.f - (float(i - i_max) + 1.f) / 5.f) * (1.f - (float(j - j_max) + 1.f) / 5.f) 
					* (float(k - k_min + 4) + 1.f) / 5.f;
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	
	
	if((xx < (number_of_subcubes_x - 1)) && (yy < (number_of_subcubes_y - 1)) && (zz < (number_of_subcubes_z - 1))) {
		
		bool neighbour_written = subimage_written[get_subcube_index(xx+1, yy, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy+1, zz, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx, yy+1, zz+1, number_of_subcubes_x, number_of_subcubes_y)]
		|| subimage_written[get_subcube_index(xx+1, yy+1, zz+1, number_of_subcubes_x, number_of_subcubes_y)];
		
		for(int k = k_max; k < k_max + 4; k++)
			for(int j = j_max; j < j_max + 4; j++)
				for(int i = i_max; i < i_max + 4; i++) {
					
					float factor = (1.f - (float(i - i_max) + 1.f) / 5.f) * (1.f - (float(j - j_max) + 1.f) / 5.f) 
					* (1.f - (float(k - k_min + 4) + 1.f) / 5.f);
				  float f_temp = factor * float(subimage.get((i-offset_x), (j-offset_y), (k-offset_z)));
					
					if(neighbour_written) f_temp += float(get(i, j, k));
					uchar uc_temp = uchar(f_temp);
					if(f_temp < 0.f) uc_temp = 0;
					if(f_temp > 255.f) uc_temp = 255;
					set(i, j, k, uc_temp);					
					
				}
	}
	// */
	
	subimage_written[get_subcube_index(xx, yy, zz, number_of_subcubes_x, number_of_subcubes_y)] = true;
	
  return 0;	
}

}
#endif


