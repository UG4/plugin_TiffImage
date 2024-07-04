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

#include "bridge/util.h"

// replace this with util_domain_dependent.h or util_algebra_dependent.h to accelerate compilation
#include "bridge/util_domain_algebra_dependent.h"

#include "common/ug_config.h"
#include "common/error.h"
#include <string>

using namespace std;
using namespace ug::bridge;

#ifdef UG_PARALLEL
#include "pcl/pcl_util.h"
#endif

#include "tiff_image_data.h"

namespace ug{
namespace IJKRasterDataPlugin{

/** 
 *  \defgroup sample_plugin Sample
 *  \ingroup plugins
 *  This is a small sample plugin.
 *  \{
 */

static const char* ijkRasterDataPluginName = "ijkRasterDataPlugin";

void PluginSaysHello()
{
#ifdef UG_PARALLEL
	pcl::SynchronizeProcesses();
	cout << "Hello, I'm your plugin on proc " <<
				pcl::ProcRank() << "." << endl;
	pcl::SynchronizeProcesses();
#else
	UG_LOG("Hello, I'm your personal plugin in serial environment!\n");
#endif
}

void CrashFct(const string& reason)
{
	UG_THROW(ijkRasterDataPluginName << " crashed: "<< reason);
}

void CrashFctFatal(const string& reason)
{
	UG_THROW(ijkRasterDataPluginName << " crashed fatally: "<< reason);
}

void PluginCrashes()
{
	try{
		CrashFct("Some funny reason");
	}
	catch(bad_alloc& err)
	{
		UG_LOG("Bad Alloc");
	}
	UG_CATCH_THROW("Something wrong in PluginCrashes");
}

void PluginCrashesFatal()
{
	try{
		CrashFctFatal("Some fatal reason");
	}
	catch(bad_alloc& err)
	{
		UG_LOG("Bad Alloc");
	}
	UG_CATCH_THROW("Something wrong in PluginCrashesFatal");
}

/**
 * Class exporting the functionality of the plugin. All functionality that is to
 * be used in scripts or visualization must be registered here.
 */
struct Functionality
{
	/**
	 * Function called for the registration of Domain and Algebra independent parts
	 * of the plugin. All Functions and Classes not depending on Domain and Algebra
	 * are to be placed here when registering.
	 *
	 * @param reg				registry
	 * @param parentGroup		group for sorting of functionality
	 */
	static void Common(Registry& reg, string grp)
	{
		/*reg.add_function("PluginSaysHello", &PluginSaysHello, grp)
			.add_function("PluginCrashes", &PluginCrashes, grp)
			.add_function("PluginCrashesFatal", &PluginCrashesFatal, grp);*/
	}


/**
 * Function called for the registration of Domain and Algebra dependent parts
 * of the plugin. All Functions and Classes depending on both Domain and Algebra
 * are to be placed here when registering. The method is called for all
 * available Domain and Algebra types, based on the current build options.
 *
 * @param reg				registry
 * @param parentGroup		group for sorting of functionality
 */
template <typename TDomain, typename TAlgebra>
static void DomainAlgebra(Registry& reg, string grp)
{
//	useful defines
	string suffix = GetDomainAlgebraSuffix<TDomain,TAlgebra>();
	string tag = GetDomainAlgebraTag<TDomain,TAlgebra>();

}

/**
 * Function called for the registration of Domain dependent parts
 * of the plugin. All Functions and Classes depending on the Domain
 * are to be placed here when registering. The method is called for all
 * available Domain types, based on the current build options.
 *
 * @param reg				registry
 * @param parentGroup		group for sorting of functionality
 */
template <typename TDomain>
static void Domain(Registry& reg, string grp)
{
//	useful defines
	string suffix = GetDomainSuffix<TDomain>();
	string tag = GetDomainTag<TDomain>();

}

/**
 * Function called for the registration of Dimension dependent parts
 * of the plugin. All Functions and Classes depending on the Dimension
 * are to be placed here when registering. The method is called for all
 * available Dimension types, based on the current build options.
 *
 * @param reg				registry
 * @param parentGroup		group for sorting of functionality
 */
template <int dim, typename TRegistry=ug::bridge::Registry>
static void Dimension(TRegistry& reg, string grp)
{
//	useful defines
	string suffix = GetDimensionSuffix<dim>();
	string tag = GetDimensionTag<dim>();

	{
		typedef TiffImageData<dim, number> T;

		string name = string("TiffImageDataNumber").append(suffix);
		reg.template add_class_<T, typename T::user_data_type>(name, grp)
    	   .template add_constructor<void (*)() >("")
		   .add_method("init", &T::init)
		   .add_method("set_corner", &T::set_corner)
		   .add_method("set_corners", &T::set_corners)
		   .add_method("get_size_x", &T::get_size_x)
		   .add_method("get_size_y", &T::get_size_y)
		   .add_method("get_size_z", &T::get_size_z)
		   .set_construct_as_smart_pointer(true);
		reg.add_class_to_group(name, "TiffImageDataNumber", tag);
	}
}

/**
 * Function called for the registration of Algebra dependent parts
 * of the plugin. All Functions and Classes depending on Algebra
 * are to be placed here when registering. The method is called for all
 * available Algebra types, based on the current build options.
 *
 * @param reg				registry
 * @param parentGroup		group for sorting of functionality
 */
template <typename TAlgebra>
static void Algebra(Registry& reg, string grp)
{
//	useful defines
	string suffix = GetAlgebraSuffix<TAlgebra>();
	string tag = GetAlgebraTag<TAlgebra>();

}

}; // end Functionality

// end group sample_plugin
/// \}

}// end of namespace Sample


/**
 * This function is called when the plugin is loaded.
 */
template <typename TRegistry=ug::bridge::Registry>
void RegisterBridge_IJKRasterData(TRegistry* reg, string grp)
{
	grp.append("/IJKRasterData");

	typedef ug::IJKRasterDataPlugin::Functionality Functionality;

	try{
	//	RegisterCommon<Functionality>(*reg,grp);
		RegisterDimensionDependent<Functionality>(*reg,grp);
	//	RegisterDomainDependent<Functionality>(*reg,grp);
	//	RegisterAlgebraDependent<Functionality>(*reg,grp);
	//	RegisterDomainAlgebraDependent<Functionality>(*reg,grp);
	}
	UG_REGISTRY_CATCH_THROW(grp);
}

#ifdef UG_USE_PYBIND11 // Expose for pybind11.
namespace IJKRasterDataPlugin
{
	void InitUGPlugin(ug::pybind::Registry* reg, string grp)
	{
		RegisterBridge_IJKRasterData<ug::pybind::Registry> (reg,grp);
	}
}
#endif

extern "C" void
InitUGPlugin_IJKRasterData(Registry* reg, string grp)
{
	RegisterBridge_IJKRasterData<Registry> (reg,grp);
}

extern "C" UG_API void
FinalizeUGPlugin_IJKRasterData()
{}

}//	end of namespace ug
