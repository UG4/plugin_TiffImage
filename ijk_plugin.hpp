/*
 * ijk_bridge.hpp
 *
 *  Created on: 04.07.2024
 *      Author: anaegel
 */

#ifndef IJK_PLUGIN_HPP_
#define IJK_PLUGIN_HPP_


#include "bridge/util.h"

extern "C" void InitUGPlugin_IJKRasterData(ug::bridge::Registry* reg, std::string grp);

#ifdef UG_USE_PYBIND11

#include "bindings/pybind/ug_pybind.h"

namespace ug {
namespace IJKRasterDataPlugin{
	void InitUGPlugin(ug::pybind::Registry* reg, std::string grp);
}
}
#endif


#endif /* IJK_PLUGIN_HPP_ */



