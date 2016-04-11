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


// extern headers
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>

// #define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>
#include <cstdlib>
#include <time.h>
#include "common/common.h"

#include "lib_disc/common/groups_util.h"
#include "lib_disc/common/function_group.h"
#include "lib_grid/tools/subset_group.h"
#include "bridge/bridge.h"
#include "lib_disc/domain.h"
#include "lib_disc/domain_traits.h"
#include "lib_algebra/cpu_algebra_types.h"
#include "lib_disc/quadrature/quadrature.h"
#include "lib_disc/local_finite_element/local_finite_element_provider.h"
#include "lib_disc/spatial_disc/disc_util/fv1_geom.h"
#include "lib_disc/spatial_disc/user_data/user_data.h"
#include <boost/function.hpp>
#include "common/util/provider.h"
#include "lib_disc/spatial_disc/disc_util/fv1_geom.h"
#include "lib_disc/spatial_disc/disc_util/hfv1_geom.h"
#include "common/parser/rapidxml/rapidxml_print.hpp"
#include "common/parser/rapidxml/rapidxml.hpp"
#include "save_load.h"
#include "common/debug_print.h"


#ifdef UG_FOR_LUA
#include "bindings/lua/lua_user_data.h"
#endif

namespace ug{
namespace ImageDataPlugin{

// TODO: Add excessively long functions here

template <int dim, typename TData=number>
void ImageData :: modify_grid(Grid &grid)
{
	// grid.
}

template <int dim, typename TData=number>
void set_corners_auto(MathVector<2> &x1)
{
	x1 = MathVector<2> ( get_size_x(),  get_size_y());
}

template <int dim, typename TData=number>
void set_corners_auto(MathVector<3> &x1)
{
	x1 = MathVector<3> ( get_size_x(),  get_size_y(),  get_size_y());
}



} // namespace ImageDataPlugin
} // namespace ug
