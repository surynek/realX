/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-053_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* network.h / 0-053_nofutu                                                   */
/*----------------------------------------------------------------------------*/
//
// Virtual network embedding model and Boolean encoding.
//
/*----------------------------------------------------------------------------*/


#ifndef __NETWORK_H__
#define __NETWORK_H__


#include <vector>

#include "defs.h"

#include "common/types.h"
#include "common/graph.h"

#include "core/cnf.h"


using namespace std;

using namespace realX;


/*----------------------------------------------------------------------------*/

namespace realX
{


/*----------------------------------------------------------------------------*/
    
    class sEmbeddingModel
    {
    public:
	typedef std::vector<s_DirectedGraph> DirectedGraphs_vector;
	typedef std::vector<sInt_32> Offsets_vector;
	typedef std::vector<sInt_32> Mapping_vector;
	typedef std::vector<Mapping_vector> Mappings_vector;	

    public:
	Glucose::Solver* setup_SATSolver(sDouble timeout = -1.0);	
	void setup_LazyModel(sBoolEncoder *encoder);
	
	void build_LazyModel(sBoolEncoder *encoder, Glucose::Solver *solver);
        bool solve_LazyModel(Glucose::Solver *solver);
	void decode_LazyModel(Glucose::Solver *solver, Mappings_vector &virutal_Embeddings);
	bool refine_LazyModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &virtual_Embeddings);

	sInt_32 calc_EmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_v_id, sInt_32 phys_v_id);
	void calc_EmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &virt_v_id, sInt_32 &phys_v_id);

    public:
	virtual void to_Screen(const sString &indent = "") const;
	virtual void to_Stream(FILE *fw, const sString &indent = "") const;

    public:		
	s_DirectedGraph m_physical_Network;
	DirectedGraphs_vector m_virtual_Networks;

	Offsets_vector m_mapping_Offsets;
    };

/*----------------------------------------------------------------------------*/
    
    class sPathEmbeddingModel
    {
    public:
	typedef std::vector<s_DirectedGraph> DirectedGraphs_vector;
	
	typedef std::vector<sInt_32> Offsets_vector;
	typedef std::vector<Offsets_vector> Offsets_2vector;
	typedef std::vector<Offsets_2vector> Offsets_3vector;	
	
	typedef Offsets_vector VertexOffsets_vector;
	typedef Offsets_3vector EdgeOffsets_vector;	

	typedef std::vector<sInt_32> PathMapping_vector;
	typedef std::vector<PathMapping_vector> NeighborPathMappings_vector;
	typedef std::vector<NeighborPathMappings_vector> VertexPathMappings_vector;
	typedef std::vector<VertexPathMappings_vector> NetworkPathMappings_vector;
	
	typedef std::vector<sInt_32> Mapping_vector;
	typedef std::vector<Mapping_vector> Mappings_vector;

    public:
	Glucose::Solver* setup_SATSolver(sDouble timeout = -1.0);	
	void setup_LazyPathModel(sBoolEncoder *encoder);
	void setup_LimitedLazyPathModel(sBoolEncoder *encoder, sInt_32 depth);	
	
	void build_LazyPathModel(sBoolEncoder *encoder, Glucose::Solver *solver);
	void build_LimitedLazyPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 depth);	
	void build_IndividualPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_v_id, sInt_32 virt_u_id, sInt_32 neighbor_index);
	void build_LimitedIndividualPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_v_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 depth);
	
	void build_IndividualCorrespondence(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_v_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_u_id, sInt_32 phys_v_id);	
	
        bool solve_LazyPathModel(Glucose::Solver *solver);
	
	void decode_LazyPathModel(Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings);
	void decode_LimitedLazyPathModel(Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings, sInt_32 depth);
	
	bool refine_LazyPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &vertex_Embeddings, const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings);	
	bool refine_LimitedLazyPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &vertex_Embeddings, const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings, sInt_32 depth);

	sInt_32 calc_VertexEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_v_id, sInt_32 phys_v_id) const;
	sInt_32 calc_EdgeEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_u_id, sInt_32 phys_v_id) const;
	
	void decode_VertexEmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &virt_v_id, sInt_32 &phys_v_id) const;
	void decode_EdgeEmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &u_id, sInt_32 &v_id, sInt_32 &neighbor_index, sInt_32 &phys_u_id, sInt_32 &path_index) const;
	void decode_LimitedEdgeEmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &u_id, sInt_32 &v_id, sInt_32 &neighbor_index, sInt_32 &phys_u_id, sInt_32 &path_index, sInt_32 depth) const;	

    public:
	virtual void to_Screen(const sString &indent = "") const;
	virtual void to_Stream(FILE *fw, const sString &indent = "") const;

    public:		
	s_DirectedGraph m_physical_Network;
	DirectedGraphs_vector m_virtual_Networks;
	
	VertexOffsets_vector m_vertex_mapping_Offsets;
	sInt_32 m_last_vertex_mapping_variable;
	sInt_32 m_last_edge_mapping_variable;	
	EdgeOffsets_vector m_edge_mapping_Offsets;	
    };    
  

/*----------------------------------------------------------------------------*/

} // namespace realX

#endif /* __NETWORK_H__ */
