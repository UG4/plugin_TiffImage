/*
 *  c3dimage_tiff.cpp
 *  
 *
 *  Created by Daniel on 20.11.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */


#include "../../TiffImage/neura2/c3dimage.h"

#ifdef NEURA2_WITH_TIFF_

#include <tiffio.h>
#include <cmath> 

namespace neura2 {
int C3DImage::read_tiff(const char * filename) {
	
  // Öffnen der Datei:
	TIFF * tiff = TIFFOpen(filename, "r");
	
	if(tiff == NULL) {
		std::cout << "File " << filename << " not found ... exiting." << std::endl;
		return 1;
	}
	
	// Überprüfen, dass das Bild im richtigen Format gespeichert ist:
	unsigned short us_photometric = 0;
	unsigned short us_bits_per_sample = 0;
	TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &us_photometric);
	TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &us_bits_per_sample);
	
	if((us_photometric == 2) || (us_bits_per_sample != 8)) {
		 std::cout << "Unsupported file format in file " << filename << " ... exiting." << std::endl;
		 return 1;
	}
	
	// Auslesen der Bildgröße (size_x und size_y) des ersten Bildes:
	TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &size_x);
	TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &size_y);
	
	// Durchgehen der einzelnen Ordner um Bildstackhöhe (size_z) zu bestimmen:
	int i_start_dir = TIFFCurrentDirectory(tiff);
	do {
		size_z ++;
		
		// Überprüfung, dass alle Bilder im Stack die gleiche Größe haben:
		int i_temp_size_x = 0;
		int i_temp_size_y = 0;
		TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &i_temp_size_x);
		TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &i_temp_size_y);
		
		if((i_temp_size_x != size_x) || (i_temp_size_y != size_y)) {
			std::cout << "File " << " has different p_images sizes in stack ... exiting." << std::endl;
			return 1;
		}
	} while(TIFFReadDirectory(tiff));
	
	
	// Allokieren von Speicher für die einzelnen Bilder:
	p_images = new CImage[size_z];
	
	if(p_images == NULL)  {
		std::cout << "Error while allocating memory for p_images in C3DImage::read_tiff ... exiting." << std::endl;
		return 1;
	}
	
  // Zurücksetzen des Tiffs auf das erste Bild:
	TIFFSetDirectory(tiff, i_start_dir);
	
	// Allokieren vom temporärem Speicher zum laden der Bilder:
	uchar * data = new uchar[size_x * size_y];
	
	if(data == NULL) {
		std::cout << "Error while allocating memory for data in C3DImage::read_tiff ... exiting." << std::endl;
		return 1;
	}
	
	// Allokieren vom temporärem Speicher zum laden der einzelnen Strips:
	long l_curr_strip_size = 1;
	uchar * p_curr_strip = new uchar[l_curr_strip_size];
	if(p_curr_strip == NULL) {
		std::cout << "Error while allocating memory for p_curr_strip in C3DImage::read_tiff ... exiting." << std::endl;
		return 1;
	}
	
	// Durchgehen der Ordner und laden der einzelnen Bilder:
  for(int zz = 0; zz < size_z; zz++) {
		
		int offset = 0;
		
		int * pl_strip_size = NULL;
		TIFFGetField(tiff, TIFFTAG_STRIPBYTECOUNTS, &pl_strip_size);
		long l_num_of_strips = TIFFNumberOfStrips(tiff);
		
		for(long counter = 0; counter < l_num_of_strips; counter ++) {
			
			// Neu allokieren des temporären Speichers für die Strips, falls notwendig:
		  if(l_curr_strip_size != pl_strip_size[counter]) {
			
				l_curr_strip_size = pl_strip_size[counter];
				//long l_curr_rows_per_strip = l_curr_strip_size / size_x;
				delete [] p_curr_strip;
				//std::cout << l_num_of_strips << " " << l_curr_strip_size << std::endl;
				p_curr_strip = new uchar[l_curr_strip_size];
				if(p_curr_strip == NULL) {
					std::cout << "Error while allocating memory for p_curr_strip in C3DImage::read_tiff ... exiting." << std::endl;
					return 1;
				}
			}
			
			// Laden der einzelnen Strips:
			TIFFReadRawStrip(tiff, counter, p_curr_strip, l_curr_strip_size);
			
				/*for(long i = 0; i < l_curr_strip_size; i++) {
				  data[offset] = uchar(p_curr_strip[i]);
					offset ++;
				}*/
			memcpy(&data[offset], p_curr_strip, l_curr_strip_size);
			offset += l_curr_strip_size;
			
			p_images[zz].set_all(data, size_x, size_y);
		}
		
		// Weitergehen zum nächsten Bild:
	  TIFFReadDirectory(tiff);
	} 
	
	// Schließen der geöffneten Datei:
	TIFFClose(tiff);

	// Freigabe des allokierten temporären Speichers:
	if(data != NULL) delete [] data;
	if(p_curr_strip != NULL) delete [] p_curr_strip;
	
	
	std::cout << "File " << filename << " has dimensions " << size_x << " x " << size_y << " x " << size_z << std::endl;

  return 0;
}



int C3DImage::write_tiff(const char * filename) {
	
	// Öffnen der Datei:
	TIFF * tiff = TIFFOpen(filename, "w");
	
	if(tiff == NULL) {
		std::cout << "Problems while writing " << filename << std::endl;
		return 1;
	}
	
	// Berechnen der Stripgrößen:
	long l_rows_per_strip = long(ceil(8192.f / float(size_x)));
	long l_num_of_strips = long(floor(float(size_y) / float(l_rows_per_strip)));
	long l_remaining_rows = 0;
	
	// Setzen der Werte für kleine Bilder:
	if(l_num_of_strips == 0) {
	  l_rows_per_strip = size_y;
		l_num_of_strips = 1;
		l_remaining_rows = 0;
	} else {
		l_remaining_rows = size_y % (l_rows_per_strip * l_num_of_strips);
	}
	
	long l_buff_size = l_rows_per_strip * size_x;
	long l_remaining_buff_size = l_remaining_rows * size_x;
	
	uchar * p_strip_buff = new uchar[l_buff_size];
	
	if(p_strip_buff == NULL) {
		std::cout << "Error while allocating memory for p_strip_buff in C3DImage::write_tiff ... exiting." << std::endl;
	  return 1;
	}
	
	uchar * p_remain_buff = NULL;
	
	if(l_remaining_rows > 0) {
		p_remain_buff = new uchar[l_remaining_buff_size];
		if(p_remain_buff == NULL) {
			std::cout << "Error while allocating memory for p_remain_buff in C3DImage::write_tiff ... exiting." << std::endl;
			return 1;
		}
	}
	

	
	// Erzeugen einer Colourmap:
	unsigned short p_grey[256];
	for(int i = 0; i < 256; i++) p_grey[i] = i * 256;
	
	for(int zz = 0; zz < size_z; zz++) {
		
		uchar * data = p_images[zz].get_pointer_to_data();
		int offset = 0;
		
		// Schreiben der einzelnen Tags für jedes Bild:
		TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, size_x);
		TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, size_y);
		TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);
		TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1);
		TIFFSetField(tiff, TIFFTAG_COMPRESSION, 1);
		TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, 1);
		TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, 3);
		TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, l_rows_per_strip);
		TIFFSetField(tiff, TIFFTAG_RESOLUTIONUNIT, 3);
		TIFFSetField(tiff, TIFFTAG_SOFTWARE, "written by class C3DImage - by daniel jungblut");
		TIFFSetField(tiff, TIFFTAG_IMAGEDESCRIPTION, "...");
		TIFFSetField(tiff, TIFFTAG_XRESOLUTION, 1.0);
		TIFFSetField(tiff, TIFFTAG_YRESOLUTION, 1.0);
		TIFFSetField(tiff, TIFFTAG_COLORMAP, p_grey, p_grey, p_grey);
		
		
		// Schreiben der einzelnen Strips:
		for(long counter = 0; counter < l_num_of_strips; counter ++) {

			memcpy(p_strip_buff, &data[offset], l_buff_size);
			offset += l_buff_size;
				
			TIFFWriteRawStrip(tiff, counter, p_strip_buff, l_buff_size);	
	  }
		
		if(l_remaining_rows > 0) {
			
			memcpy(p_remain_buff, &data[offset], l_remaining_buff_size);
			offset += l_remaining_buff_size;
			
			TIFFWriteRawStrip(tiff, l_num_of_strips, p_remain_buff, l_remaining_buff_size);
			
		}
		
		// Weitergehen zum nächsten Bild:
	  TIFFWriteDirectory(tiff);
	}
	
	
	// Freigeben des allokierten Speichers;
	if(p_strip_buff != NULL) delete [] p_strip_buff;
	if(p_remain_buff != NULL) delete [] p_remain_buff;
	
	// Schließen der geöffneten Datei:
	TIFFClose(tiff);
	
	std::cout << filename << " written successfully." << std::endl;
	
  return 0;	
}
}


#endif

