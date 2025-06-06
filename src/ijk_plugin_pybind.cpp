/*
 * ijk_plugin_pybind.cpp
 *
 *  Created on: 02.07.2024
 *      Author: anaegel
 */

#include <iostream>

#include "ijk_plugin.hpp"

#ifdef UG_USE_PYBIND11


PYBIND11_MODULE(pyijkdata, m)
{
	m.doc() = "IJKData module";
	m.attr("__name__") = "ug4py.ijkdata";

	ug::pybind::Registry registry(m);
	std::string name("IJKData");

	ug::IJKRasterDataPlugin::InitUGPlugin(&registry, name);
}

#endif
