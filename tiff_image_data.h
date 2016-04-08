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


#ifndef TIFF_IMAGE_DATA_H
#define TIFF_IMAGE_DATA_H


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


#include "../TiffImage/neura2/c3dimage.h"

namespace ug{
namespace ImageDataPlugin{




class ImageContainer : public neura2::C3DImage
{
public:
	typedef neura2::C3DImage base_type;

	ImageContainer()
	: base_type()
	{}

};

SmartPtr<ImageContainer> CreateTiffImageContainer(char *filename)
{
	SmartPtr<ImageContainer> spImgContainer = make_sp(new ImageContainer());

	spImgContainer->read_tiff(filename);

	return spImgContainer;
};

/*
void CreateImageBasedGrid(Grid& grid, char *filename)
{

}
*/
template <int dim, typename TData=number>
class ImageData : public StdGlobPosData<ImageData<dim, TData>, TData, dim, void>
{

public:

	ImageData() : m_x0(0.0), m_x1(1.0), m_image_data(NULL)
	{}

	virtual ~ImageData()
	{}


public:
	// evaluate field at pos x
	inline void evaluate(TData& val, const MathVector<dim>& x, number time, int si) const
	{

		// TODO:
		// a) determine values in closest corners
		// b) compute multi-linear combination

		val = 0.0;
	}

	void set_corners(const MathVector<dim>& x1)
	{
		m_x1 = x1;
	}


protected:

	//
	void read_data (char *filename)
	{

		// read data
		m_image_data = CreateTiffImageContainer(filename);

		// read dimensions
		int m_num_points[dim];
		m_num_points[0]= get_size_x();
		m_num_points[1]= get_size_y();

		if (dim>2) {
			m_num_points[2]= get_size_z();
		}
	}

	double interpolate(const MathVector<dim>& x)
	{
		//
		MathVector<dim> ijkpos;
		coord2ijk(x, ijkpos);


		int i,j,k;
		i = (int) floor(ijkpos[0]);
		j = (int) floor(ijkpos[1]);
		k = (int) floor(ijkpos[2]);


		unsigned char u[8];
		u[0] = m_image_data->get(i,j,k);
		u[1] = m_image_data->get(i+1,j,k);
		u[2] = m_image_data->get(i+1,j+1,k);
		u[3] = m_image_data->get(i,j+1,k);

		u[4] = m_image_data->get(i,j,k+1);
		u[5] = m_image_data->get(i+1,j,k+1);
		u[6] = m_image_data->get(i+1,j+1,k+1);
		u[7] = m_image_data->get(i,j+1,k+1);

		return u[0];
	}


	// maps coordinate to (continuous) 'ijk'
	void coord2ijk(const MathVector<dim>& coord, MathVector<dim>& ijk) const
	{

		int nx =  get_size_x();
		int ny = get_size_y();
		int nz = m_image_data->get_size_z();

		ijk[0] = (coord[0]-m_x0[0])/(m_x1[0]-m_x0[0])*nx;
		ijk[1] = (coord[1]-m_x0[1])/(m_x1[1]-m_x0[1])*ny;
		ijk[2] = (coord[2]-m_x0[2])/(m_x1[2]-m_x0[2])*nz;
	}


	// short cuts
	int get_size_x() const { return m_image_data->get_size_x(); }
	int get_size_y() const { return m_image_data->get_size_y(); }
	int get_size_z() const { return m_image_data->get_size_z(); }

protected:

	MathVector<dim> m_x0; 		// lower left corner
	MathVector<dim> m_x1; 		// upper right corner

	SmartPtr<ImageContainer> m_image_data;

};

/*
template <typename TData, int dim, typename TRet = void>
class TiffImageData
		: public StdGlobPosData<TiffImageData<TData, dim, TRet>, TData, dim, TRet>
{

public:

	TiffImageData(){

	};
	virtual ~TiffImageData()
	{


	}
	inline TRet evaluate(TData& D, const MathVector<dim>& x, number time, int si) const;


private:




};
*/




} // namespace ImageDataPlugin
} // namespace ug

//#include "tiff_image_data_impl.h"

#endif /* TIFF_IMAGE_DATA_H */
