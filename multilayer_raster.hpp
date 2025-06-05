/*
 * multilayer_raster.hpp
 *
 *  Created on: 04.06.2025
 *      Author: anaegel
 */

#ifndef MULTILAYER_RASTER_HPP_
#define MULTILAYER_RASTER_HPP_


#include "common/node_tree/node_tree.h" // for BoundingBox

#include "common/node_tree/octree.h" // for BoundingBox

namespace ug {
//! This object implements a multi-level raster data.
/*!
	*/
template < typename TData, int dim>
class MultilevelRasterData
	: public StdUserData<MultilevelRasterData<TData, dim>, TData, dim>

{
	public:
		typedef CplUserData<TData, dim> base_type;

		MultilevelRasterData(){};

		// Implement StdUserData.
		void operator() (TData& value,
								 const MathVector<dim>& globIP,
								 number time, int si) const override
		{
			// TODO
			UG_ASSERT(0, "ERROR: Implement!");
		};

		// Implement StdUserData.
		void operator()(TData vValue[],
								const MathVector<dim> vGlobIP[],
								number time, int si, const size_t nip) const override
		{
			// TODO
			UG_ASSERT(0, "ERROR: Implement!");
		};


		///	Implement ICplUserData.
		void compute(LocalVector* u, GridObject* elem,
		        const MathVector<dim> vCornerCoords[], bool bDeriv = false) override
		{
			// TODO
			UG_ASSERT(0, "ERROR: Implement!");

			const number t = this->time();
			const int si = this->subset();

	/*
			for(size_t s = 0; s < this->num_series(); ++s)
				for(size_t ip = 0; ip < this->num_ip(s); ++ip)
					this->getImpl().evaluate(this->value(s,ip), this->ip(s, ip), t, si);
					*/
		}

		// Compute values for an IP series
		void compute(LocalVectorTimeSeries* u,
		                     GridObject* elem,
		                     const MathVector<dim> vCornerCoords[],
		                     bool bDeriv = false) override
		{
			// TODO
			UG_ASSERT(0, "ERROR: Implement!");
			const int si = this->subset();

		/*
			for(size_t s = 0; s < this->num_series(); ++s)
				for(size_t ip = 0; ip < this->num_ip(s); ++ip)
					this->getImpl().evaluate(this->value(s,ip), this->ip(s, ip), this->time(s), si);
		*/
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
			// TODO
			const ReferenceObjectID roid = elem->reference_object_id();

			// typedef geometry_traits<GridObject>::Descriptor TObjDesc;

			uint nvertices = 0;
			MathVector<dim> ll, ur;


			if (roid == ROID_TRIANGLE)
			{
				node_tree::CalculateBoundingBox(ll, ur, vCornerCoords, 3);
				nvertices = 3;
			}
			else if (roid == ROID_QUADRILATERAL){
				node_tree::CalculateBoundingBox(ll, ur, vCornerCoords, 4);
				nvertices = 4;
			}

			std::cerr << "I am elem " << elem << "(" << roid << "). " << "with corners:" << std::endl;
			for (int i=0; i< nvertices;  ++i)
			{
				std::cerr <<vCornerCoords[i] << std::endl;
			}
			std::cerr << "and bounding-box "<< ll << ur << std::endl;

			std::cerr << "requested for " << std::endl;




			UG_ASSERT(0, "ERROR: Implement!");
		}

		bool requires_grid_fct() const override {return false;}
		bool constant() const override {return false;}
		bool continuous() const override {return false;}

	/*protected:
		///	access to implementation
			TImpl& getImpl() {return static_cast<TImpl&>(*this);}

		///	const access to implementation
			const TImpl& getImpl() const {return static_cast<const TImpl&>(*this);}*/
};



}


#endif /* MULTILAYER_RASTER_HPP_ */
