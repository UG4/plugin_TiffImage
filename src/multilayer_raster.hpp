/*
 * multilayer_raster.hpp
 *
 *  Created on: 04.06.2025
 *      Author: anaegel
 */

#ifndef MULTILAYER_RASTER_HPP_
#define MULTILAYER_RASTER_HPP_


#include "element_user_data.hpp"

namespace ug {

//! This is the real implementation.
template <int dim>
struct SimpleStrategy : public ElemenEvalStrategy<SimpleStrategy<dim>, dim>
{
public:
	using base_type = ElemenEvalStrategy<SimpleStrategy<dim>, dim>;
	SimpleStrategy(){}

	void prepare_element_impl(const GridObject* elem, const MathVector<dim> vCornerCoords[])
	{
		BoundingBox<dim> box(elem,vCornerCoords);
		bb[0] = box.ll;
		bb[1] = box.ur;
	}

	void eval_on_element_impl(double &value,  const MathVector<dim> &vGlobIP, number time, int si) const
	{
		// dummy return.
		value = vGlobIP[0]*vGlobIP[dim-1];
	}

protected:
	MathVector<dim> bb[2];
};



//! This object implements a multi-level raster data.
template < typename TData, int dim>
class MultilevelRasterData : public ElementUserData<TData,SimpleStrategy<dim>, dim>
{
public:
	using base_type = ElementUserData<TData,SimpleStrategy<dim>, dim>;
	MultilevelRasterData() :
		strategy(), ElementUserData<TData,SimpleStrategy<dim>, dim>(strategy)
	{}

protected:
	SimpleStrategy<dim> strategy;

};



} // namespace ug


#endif /* MULTILAYER_RASTER_HPP_ */
