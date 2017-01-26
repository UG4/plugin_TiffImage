/*
 * Copyright (c) 2011-2016:  G-CSC, Goethe University Frankfurt
 * Author: Arne Naegel
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


#ifndef _IJKPLUGIN__TIFF_IMAGE_DATA_H
#define _IJKPLUGIN__TIFF_IMAGE_DATA_H


// extern headers
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <vector>

#include "common/common.h"
#include "lib_disc/spatial_disc/user_data/user_data.h"
#include "lib_disc/quadrature/quadrature_provider.h"
#include "lib_disc/reference_element/reference_mapping_provider.h"

#ifdef UG_FOR_LUA
#include "bindings/lua/lua_user_data.h"
#endif


#include "neura2/c3dimage.h"

namespace ug {

namespace IJKRasterDataPlugin {


typedef neura2::C3DImage ImageContainer;

/*class ImageContainer : public neura2::C3DImage
{
public:
	typedef neura2::C3DImage base_type;

	ImageContainer()
	: base_type()
	{

	}

};*/

SmartPtr<ImageContainer> CreateTiffImageContainer(const char *filename)
{
	SmartPtr<ImageContainer> spImgContainer = make_sp(new ImageContainer());

	spImgContainer->read_tiff(filename);
	spImgContainer->write_tiff("debugOut.tif");
	return spImgContainer;
};




/*
 * For treating image stacks.
 *
 * Assumptions: Base data is of size NX x NY x NZ
 *
 * NX = NY = 2**p
 *
 * */


template <int dim, typename TData=number>
class TiffImageData : public StdGlobPosData<TiffImageData<dim, TData>, TData, dim, void>
{

public:

	typedef StdGlobPosData<TiffImageData<dim, TData>, TData, dim, void> base_type;
	typedef UserData<TData, dim> user_data_type;

	TiffImageData() : m_x0(0.0), m_x1(1.0), m_image_stack(SPNULL)
	{}

	virtual ~TiffImageData()
	{}


public:
	//! evaluate field at (physical) pos x
	inline void evaluate(TData& val, const MathVector<dim>& x, number time, int si) const
	{

		// TODO:
		// a) determine values in closest corners
		// b) compute multi-linear combination

		// find matching cell
		val = get_cell_data(x); ///255.0;
	}

	void set_corner(const MathVector<dim>& x1)
	{ m_x1 = x1; }

	void set_corners(const MathVector<dim>& x0, const MathVector<dim>& x1)
	{ m_x0 = x0; m_x1 = x1;}


	//! read data from file
	void init(std::string filename)
	{
		read_data(filename.c_str());
	}

	// short cuts
	int get_size_x() const { return m_image_stack->get_size_x(); }
	int get_size_y() const { return m_image_stack->get_size_y(); }
	int get_size_z() const { return m_image_stack->get_size_z(); }

protected:

	template <typename VT> void set_corners_auto(VT &x) {}


	//! read data from file
	void read_data (const char* filename)
	{
		m_image_stack = CreateTiffImageContainer(filename);
		set_corners_auto(m_x1);
	}

	//! return cell data for a given physical coordinate
	double get_cell_data(const MathVector<dim>& x) const
	{

		UG_ASSERT(m_image_stack.valid(), "Need to initialize data set first!")
		//
		MathVector<dim> ijkpos;
		coord2ijk(x, ijkpos);

		int i = (int) floor(ijkpos[0]);
		int j = (int) floor(ijkpos[1]);
		int k = (int) floor(ijkpos[2]);

		UG_ASSERT( i < get_size_x() && i>= 0, "Invalid x: " << i << ", " << x[0] << ", " << m_x1);
		UG_ASSERT( j < get_size_y() && j>= 0, "Invalid y: " << j << ", " << x[1] << ", " << m_x1);
		UG_ASSERT( k < get_size_z() && k>= 0, "Invalid z: " << k << ", " << x[2] << ", " << m_x1);

		/*if (i+1==get_size_x() && j+1==get_size_y())
		{
			UG_LOG(get_size_x() << " " << get_size_y() << " "<< i+j*get_size_x());
		}
*/
		//UG_LOG(i << " " << j << " "<< i+j*get_size_x() << std::endl);

		return m_image_stack->get(i,j,k);
		//return i+j*get_size_x();
		//const_cast<ImageData<dim, TData>* >(this)->dummy++;
		//return (dummy);
		/*
		unsigned char u[8];
		u[0] = m_image_stack->get(i,j,k);
		u[1] = m_image_stack->get(i+1,j,k);
		u[2] = m_image_stack->get(i+1,j+1,k);
		u[3] = m_image_stack->get(i,j+1,k);

		u[4] = m_image_stack->get(i,j,k+1);
		u[5] = m_image_stack->get(i+1,j,k+1);
		u[6] = m_image_stack->get(i+1,j+1,k+1);
		u[7] = m_image_stack->get(i,j+1,k+1);
		*/
	}


	//! map physical coordinate to (continuous) ijk coordinate
	void coord2ijk(const MathVector<dim>& coord, MathVector<dim>& ijk) const
	{

		int nx = get_size_x();
		int ny = get_size_y();
		int nz = get_size_z();

		ijk[0] = (coord[0]-m_x0[0])/(m_x1[0]-m_x0[0])*nx;
		ijk[1] = (coord[1]-m_x0[1])/(m_x1[1]-m_x0[1])*ny;
		ijk[2] = (coord[2]-m_x0[2])/(m_x1[2]-m_x0[2])*nz;
	}

protected:

	MathVector<dim> m_x0; 		// lower left corner
	MathVector<dim> m_x1; 		// upper right corner

	SmartPtr<ImageContainer> m_image_stack;

};






} // namespace IJKRasterDataPlugin
} // namespace ug

//#include "tiff_image_data_impl.h"

#endif /* TIFF_IMAGE_DATA_H */
