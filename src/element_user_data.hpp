/*
 * element_user_data.hpp
 *
 *  Created on: 11.06.2025
 *      Author: anaegel
 */

#ifndef SRC_ELEMENT_USER_DATA_HPP_
#define SRC_ELEMENT_USER_DATA_HPP_


#include "common/node_tree/node_tree.h" // for BoundingBox
#include "common/node_tree/octree.h" // for BoundingBox

namespace ug {



//! Bounding box for an element.
template <int dim>
struct BoundingBox
{
	BoundingBox(const GridObject* elem, const MathVector<dim> vCornerCoords[])
	{
		const ReferenceObjectID roid = elem->reference_object_id();
		uint nvertices = 0;

		if (roid == ROID_TRIANGLE) {
			node_tree::CalculateBoundingBox(ll, ur, vCornerCoords, 3);
			nvertices = 3;
		} else if (roid == ROID_QUADRILATERAL){
			node_tree::CalculateBoundingBox(ll, ur, vCornerCoords, 4);
			nvertices = 4;
		}

		std::cerr << "This is elem " << elem << "(" << roid << ") " << "with corners:" << std::endl;
		for (int i=0; i< nvertices;  ++i)
		{
			std::cerr <<vCornerCoords[i] << std::endl;
		}
		std::cerr << "and bounding-box given by "<< ll <<" and " << ur <<"." << std::endl;
	}

	MathVector<dim> ll;
	MathVector<dim> ur;
};


//! This is the base class for strategies
template <typename TDerived, int dim>
struct ElemenEvalStrategy
{

	//! Call this function to prepare element.
	void prepare_element(const GridObject* elem, const MathVector<dim> vCornerCoords[])
	{
		static_cast<TDerived*>(this)->prepare_element_impl(elem, vCornerCoords);
	};

	void eval_on_element(double &value, const MathVector<dim> &vGlobIP, number time, int si) const
	{
		static_cast<const TDerived*>(this)->eval_on_element_impl(value, vGlobIP, time, si);
	}

};


//! This object implements a multi-level raster data.
/*! It relies on defining a Strategy for concrete implementation
 * */
template <typename TData, typename TElemEvalStrategy, int dim>
class ElementUserData
	: public StdUserData<ElementUserData<TData, TElemEvalStrategy, dim>, TData, dim>

{
	public:
		typedef CplUserData<TData, dim> base_type;
		typedef TElemEvalStrategy TStrategy;

		ElementUserData(TStrategy &s) : strategy(s) {}
		TStrategy &strategy;

		//! Implement StdUserData.
		void operator() (TData& value,
								 const MathVector<dim>& globIP,
								 number time, int si) const override
		{
			// TODO: Add code!
			std::cerr << __FILE__ << ":" <<__LINE__ << std::endl;
			// ElemenEvalStrategy::eval_at_points(&value, 1, &globIP, time, si);
			UG_ASSERT(0, "ERROR: Implement!");
		};

		// Implement StdUserData.
		void operator()(TData vValue[],
								const MathVector<dim> vGlobIP[],
								number time, int si, const size_t nip) const override
		{
			// TODO: Add code!
			std::cerr << __FILE__ << ":" <<__LINE__ << std::endl;
			// ElemenEvalStrategy::eval_at_points(vValue, nip, vGlobIP, time, si);
			UG_ASSERT(0, "ERROR: Implement!");
		};


		//! Implement ICplUserData.
		void compute(LocalVector* u, GridObject* elem,
		        const MathVector<dim> vCornerCoords[], bool bDeriv = false) override
		{

			std::cerr << __FILE__ << ":" <<__LINE__ << std::endl;
			strategy.prepare_element(elem, vCornerCoords);

			const number t = this->time();
			const int si = this->subset();

			for(size_t s = 0; s < this->num_series(); ++s)
				for(size_t ip = 0; ip < this->num_ip(s); ++ip)
				{
					// Evaluation point.
					auto x = this->ip(s, ip);
					strategy.eval_on_element(this->value(s,ip), x, t, si);
				}

		}

		//! Compute values for an IP series
		void compute(LocalVectorTimeSeries* u,
		                     GridObject* elem,
		                     const MathVector<dim> vCornerCoords[],
		                     bool bDeriv = false) override
		{
			std::cerr << __FILE__ << ":" <<__LINE__ << std::endl;
			strategy.prepare_element(elem, vCornerCoords);


			for(size_t s = 0; s < this->num_series(); ++s)
				for(size_t ip = 0; ip < this->num_ip(s); ++ip)
					strategy.eval_on_element(this->value(s,ip), this->ip(s, ip), this->time(s), this->subset());

		}


		//! Implement for StdUserData.
		template <int refDim>
		inline void evaluate(TData vValue[],
		                     const MathVector<dim> vGlobIP[],
		                     number time, int si,
		                     GridObject* elem,
		                     const MathVector<dim> vCornerCoords[],
		                     const MathVector<refDim> vLocIP[],
		                     const size_t nip,
		                     LocalVector* u,
		                     const MathMatrix<refDim, dim>* vJT = NULL) const
		{

			std::cerr << __FILE__ << ": " <<__LINE__ << std::endl;
			strategy.prepare_element(elem, vCornerCoords);

			for (int i=0; i<nip; ++i)
			{
				strategy.eval_on_element(vValue[i], vGlobIP[i], time, si);
			}


		}

		bool requires_grid_fct() const override {return false;}
		bool constant() const override {return false;}
		bool continuous() const override {return false;}

};




} // namespace ug


#endif /* MULTILAYER_RASTER_HPP_ */
