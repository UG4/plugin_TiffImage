/*
 * py_element_user_data.hpp
 *
 *  Created on: 26.07.2025
 *      Author: anaegel
 */

#ifndef PYTHON_ELEM_USER_DATA_HPP_
#define PYTHON_ELEM_USER_DATA_HPP_


#ifdef UG_USE_PYBIND11
#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

#include "bindings/pybind/python_user_data.h"

namespace py = pybind11;



#include "element_user_data.hpp"

namespace ug {




// Need to evaluate a python function

template<int dim>
struct PyElemDataTraits
{
	typedef typename std::function<double(double,int)> TCppFunction;
	template <typename TFunction>
	static py::object call(TFunction f, MathVector<dim>[3], double t, int si)
	{return f(t,si);}
};

template<>
struct PyElemDataTraits<1>
{
	typedef typename std::function<double(double, double, int)> TCppFunction;

	template <typename TFunction>
	static py::object  call(TFunction f, const MathVector<1> x, const MathVector<1> bb0, const MathVector<1> bb1,  double t, int si)
	{return f(x[0],bb0[0],bb1[0],t,si);}
};

template<>
struct PyElemDataTraits<2>
{
	typedef typename std::function<double(double, double, double, int)> TCppFunction;
	template <typename TFunction>
	static py::object call(TFunction f, const MathVector<2> x, const MathVector<2> bb0, const MathVector<2> bb1, double t, int si)
	{return f(x[0],x[1], bb0[0],bb0[1],bb1[0],bb1[1],t,si);}
};

template<>
struct PyElemDataTraits<3>
{
	typedef typename std::function<double(double, double, double, double, int)> TCppFunction;

	template <typename TFunction>
	static py::object call(TFunction f, const MathVector<3> x, const MathVector<3> bb0, const MathVector<3> bb1, double t, int si)
	{ return f(x[0],x[1],x[2], bb0[0],bb0[1],bb0[2], bb1[0],bb1[1],bb1[2],t,si); }
};

//! This realizes a Python callback.
template <int dim>
struct PyCallbackStrategy : public ElemenEvalStrategy<PyCallbackStrategy<dim>, dim>
{

	typedef py::object TFunction;
	PyCallbackStrategy(TFunction f) : pyfunc(f) {}

	void prepare_element_impl(const GridObject* elem, const MathVector<dim> vCornerCoords[])
	{
		BoundingBox<dim> box(elem,vCornerCoords);
		bb[0] = box.ll;
		bb[1] = box.ur;
	}

	//! Evaluate at x for a given bounding box B(x) at time t and subset si.
	void eval_on_element_impl(double &value,  const MathVector<dim> &vGlobIP, number time, int si) const
	{
		typedef number TData;
		typedef void TRet;
		py::object result_py = PyElemDataTraits<dim>().call(pyfunc, vGlobIP, bb[0], bb[1], time, si);
		value = PyUserDataTypeTraits<TData, TRet>::data_value(result_py);
		// return PyUserDataTypeTraits<TData, TRet>::return_value(result_py);
	}

protected:
	MathVector<dim> bb[2];

	TFunction pyfunc; //! Callback function
};


template < typename TData, int dim>
class PyElementUserData : public ElementUserData<TData,PyCallbackStrategy<dim>, dim>
{
public:
	typedef py::object TFunction;

	PyElementUserData(TFunction f) :
	strategy(f), ElementUserData<TData,PyCallbackStrategy<dim>, dim>(strategy)
	{}

protected:
	PyCallbackStrategy<dim> strategy;
};



} // namespace ug

#endif
#endif /* MULTILAYER_RASTER_HPP_ */
