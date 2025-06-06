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


#ifndef _NEURA2__CIMAGE_H_
#define _NEURA2__CIMAGE_H_

#include <string.h>
#include <cstdlib>
#include <cstdio>

#include "constants.h"

namespace neura2 {
// Klasse zum Speichern von einzelnen Bildebenen, verwendet von Klasse C3DImage:
class CImage {
	
private:
	
	int size_x;    // Dimension des Bildes in x-Richtung
	int size_y;    // Dimension des Bildes in y-Richtung
	
	uchar * p_data;  // Feld, in dem die Bilddaten zeilenweise abgelegt werden
	
	
	// Instanzen vom Typ CImage sollen nicht kopiert werden (siehe Effective C++, Item 6, 3rd Edition)
	CImage (const CImage&);
	CImage& operator=(const CImage&);
	
	
public:
	
	// Standard-Konstruktor erzeugt ein leeres Bild:
	CImage() : size_x(0), size_y(0), p_data(NULL) {}

	
	// Destruktor gibt allokierten Speicher wieder frei:
	~CImage() {
		if(p_data) delete [] p_data;
	}
	
	
	// Allokiert Speicherplatz und lädt das Bild aus dem Stream file:
	int read(FILE * & file, const int size_x, const int size_y) {
		
		this->size_x = size_x;
		this->size_y = size_y;
		
		p_data = new uchar[this->size_x * this->size_y];
		
		if(p_data == NULL) {
			std::cout << "Problems while allocating memory in CImage::read ... exiting." << std::endl;
			return 1;
		}
		
		if(fread(p_data, sizeof(uchar), this->size_x * this->size_y, file) == 0) {
			std::cout << "Problems while reading from file in CImage::read ... exiting." << std::endl;
  		return 1;
	  }
		
		return 0;
  }
		
	
	// Das Bild wird in den Stream file geschrieben:
	// Rückgabewert 0 bei Erfolg, 1 bei Misserfolg.
	int write(FILE * & file) {
		
	  if((p_data != NULL) && (fwrite(p_data, sizeof(uchar), size_x * size_y, file))) 
      return 0;
		else return 1;
	}
	
	void setSize(const int size_x, const int size_y) {
		
	  this->size_x = size_x;
		this->size_y = size_y;
		p_data = new uchar[size_x * size_y];
		
	}
		
	
	// Gibt den Bildwert an der Stelle i, j zurück.
	// Bei nicht-validen Werten für i oder j ist die Rückgabe 0.
	uchar get(const int i, const int j) const {
		if((p_data != NULL) && (i >= 0) && (i < size_x) && (j >= 0) && (j < size_y))
			return p_data[j * size_x + i];
		else return uchar(0);
	}
	
	// Setzt den Bildwert an der Stelle i, j auf den Wert u.
	// Rückgabewert 0 bei Erfolg, 1 für nicht-valide Werte für i oder j.
	int set(const int i, const int j, uchar u) {
	  if((p_data != NULL) && (i >= 0) && (i < size_x) && (j >= 0) && (j < size_y)) {
			p_data[j * size_x + i] = u;
			return 0;
		} else return 1;
		
	}
	
	
	// Allokiert Speicher und kopiert das übergebene Feld in den allokierten Speicherbereich:
	int set_all(const uchar * const p_data, const int size_x, const int size_y) {
	
		this->size_x = size_x;
		this->size_y = size_y;
		
		this->p_data = new uchar[this->size_x * this->size_y];
		
		if(p_data == NULL) {
			std::cout << "Problems while allocating memory in CImage::set_all ... exiting." << std::endl;
			return 1;
		}
		
		if(memcpy(this->p_data, p_data, this->size_x * this->size_y * sizeof(uchar)) == NULL) {
			std::cout << "Problems while copying p_data in CImage::set_all ... exiting." << std::endl;
		}
		
		return 0;
	}
	
	
	// CImage stellt Zeiger zu den Bilddaten bereit, um einen schnellen Zugriff darauf
	// zu gewährleisten. Die Daten dürfen von außerhalb keinesfalls gelöscht werden!
	uchar * get_pointer_to_data() {
	  return p_data;
	}
		
		
};
	
}
#endif
