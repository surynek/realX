/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-133_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* network.cpp / 0-133_nofutu                                                 */
/*----------------------------------------------------------------------------*/
//
// Robot (model) related data structures and functions.
//
/*----------------------------------------------------------------------------*/


#include <stdio.h>
#include <math.h>

#include "core/network.h"


using namespace realX;




/*----------------------------------------------------------------------------*/

namespace realX
{



    
/*============================================================================*/
// sEmbeddingModel class
/*----------------------------------------------------------------------------*/

    Glucose::Solver* sEmbeddingModel::setup_SATSolver(sDouble timeout)
    {
	Glucose::Solver *solver;
	solver = new Glucose::Solver;

	solver->s_Glucose_timeout = timeout;
	solver->setIncrementalMode();

	return solver;
    }


    void sEmbeddingModel::destroy_SATSolver(Glucose::Solver *solver)
    {
	delete solver;
    }

    
    void sEmbeddingModel::setup_LazyModel(sBoolEncoder *encoder)
    {
	sInt_32 offset = 1;
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_mapping_Offsets.push_back(offset);
	    offset += m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();
	}
	encoder->set_LastVariableID(offset);	
    }

   
    void sEmbeddingModel::build_LazyModel(sBoolEncoder *encoder, Glucose::Solver *solver)
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sBoolEncoder::VariableIDs_vector virt_to_phys_IDs;
		    
		for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
		{
		    sInt_32 virt_to_phys_id = calc_EmbeddingBitVariableID(vn_id, virt_v_id, phys_v_id);
		    virt_to_phys_IDs.push_back(virt_to_phys_id);
			    
		    if (m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_in_Neighbors.size() <= m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.size())
		    {
			
			for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_in_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_in_Neighbors.end(); ++virt_neighbor)
			{
			    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
			    sBoolEncoder::VariableIDs_vector potential_target_IDs;
			    sInt_32 potential_mapping_id = calc_EmbeddingBitVariableID(vn_id, virt_v_id, phys_v_id);
			    
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.end(); ++phys_neighbor)
			    {
				sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;				
				sInt_32 potential_target_id = calc_EmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_neighbor_id);
				potential_target_IDs.push_back(potential_target_id);
			    }
			    encoder->cast_MultiImplication(solver, potential_mapping_id, potential_target_IDs);
			}
		    }
		    if (m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.size() <= m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size())
		    {			
			for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
			{
			    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
			    sBoolEncoder::VariableIDs_vector potential_target_IDs;
			    sInt_32 potential_mapping_id = calc_EmbeddingBitVariableID(vn_id, virt_v_id, phys_v_id);
			    
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
			    {
				sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;				
				sInt_32 potential_target_id = calc_EmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_neighbor_id);
				potential_target_IDs.push_back(potential_target_id);
			    }
			    encoder->cast_MultiImplication(solver, potential_mapping_id, potential_target_IDs);
			}			
		    }		    
		    else /* trivial vertex degree mismatch */
		    {
			sInt_32 mapping_id = calc_EmbeddingBitVariableID(vn_id, virt_v_id, phys_v_id);
			encoder->cast_BitUnset(solver, mapping_id);
		    }
		}
		encoder->cast_Disjunction(solver, virt_to_phys_IDs);
		encoder->cast_AdaptiveAllMutexConstraint(solver, virt_to_phys_IDs);
	    }
	}
    }


    bool sEmbeddingModel::solve_LazyModel(Glucose::Solver *solver)
    {
	if (!solver->simplify())
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return false;
	}
	Glucose::lbool result = solver->solve_();

	if (result == l_True)
	{
    	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_satisfiable_SAT_solver_Calls;
	    }
	    #endif	    
	}
	else if (result == l_False)
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif	    	    
	    return false;
	}
	else if (result == l_Undef)
	{
	    return false;
	}
	else
	{
	    sASSERT(false);
	}
	
	return true;	
    }

    
    void sEmbeddingModel::decode_LazyModel(Glucose::Solver *solver, Mappings_vector &virtual_Embeddings)
    {
	virtual_Embeddings.resize(m_virtual_Networks.size());
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    virtual_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	}
	
	for (sInt_32 i = 0; i < solver->nVars(); i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {	    
		sInt_32 vnet_id, virt_v_id, phys_v_id;
		sInt_32 variable_ID = sABS(literal);

		calc_EmbeddingMapping(variable_ID, vnet_id, virt_v_id, phys_v_id);

		virtual_Embeddings[vnet_id][virt_v_id] = phys_v_id;
	    }
	}
    }


    bool sEmbeddingModel::refine_LazyModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &virtual_Embeddings)
    {
	bool refined = false;
	
	vector<sDouble> vertex_Saturations;
	vector<sBoolEncoder::VariableIDs_vector> vertex_Nogoods;
	
	vertex_Saturations.resize(m_physical_Network.get_VertexCount(), 0.0);
	vertex_Nogoods.resize(m_physical_Network.get_VertexCount());
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();
	    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
//		printf("%d <=+ %.3f\n", virtual_Embeddings[vn_id][virt_v_id], m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity);
		
		vertex_Saturations[virtual_Embeddings[vn_id][virt_v_id]] += m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity;
//		printf("  %d,%d,%d\n", vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]);
		vertex_Nogoods[virtual_Embeddings[vn_id][virt_v_id]].push_back(calc_EmbeddingBitVariableID(vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]));
	    }
	}
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
//	    printf("Saturs %d: %.3f\n", phys_v_id, vertex_Saturations[phys_v_id]);
	    if (vertex_Saturations[phys_v_id] > m_physical_Network.m_Vertices[phys_v_id].m_capacity + s_EPSILON)
	    {
		refined = true;

		/*		
		printf("refining...\n");
		for (sInt_32 var_id = 0; var_id < vertex_Nogoods[phys_v_id].size(); ++var_id)
		{
		    printf("    %d\n", vertex_Nogoods[phys_v_id][var_id]);
		}
		*/

		encoder->cast_BigMutex(solver, vertex_Nogoods[phys_v_id]);
//		encoder->cast_Mutex(solver, vertex_Nogoods[phys_v_id][0], vertex_Nogoods[phys_v_id][1]);
	    }
	}
	return refined;
    }
	    

    sInt_32 sEmbeddingModel::calc_EmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_v_id, sInt_32 phys_v_id)
    {
	sASSERT(!m_mapping_Offsets.empty());
	
	return (m_mapping_Offsets[vnet_id] + virt_v_id * m_physical_Network.get_VertexCount() + phys_v_id);
    }


    void sEmbeddingModel::calc_EmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &virt_v_id, sInt_32 &phys_v_id)
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    sInt_32 first_ID = m_mapping_Offsets[vn_id];
	    sInt_32 last_ID = first_ID + m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();

	    if (variable_ID >= first_ID && variable_ID < last_ID)
	    {
		vnet_id = vn_id;
		sInt_32 remainder = variable_ID - first_ID;

		virt_v_id = remainder / m_physical_Network.get_VertexCount();
		phys_v_id = remainder % m_physical_Network.get_VertexCount();
		
		return;
	    }
	}	
    }
    
    /*
	for (sInt_32 i = 0; i < solver->nVars(); i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 variable_ID = sABS(literal);
    
    */


    /*
    */
    
/*----------------------------------------------------------------------------*/
    
    void sEmbeddingModel::to_Screen(const sString &indent) const
    {
	to_Stream(stdout, indent);
    }

    
    void sEmbeddingModel::to_Stream(FILE *fw, const sString &indent) const
    {
	fprintf(fw, "%sEmbedding Model: {\n", indent.c_str());
	
	fprintf(fw, "%s%sphysical network:\n", indent.c_str(), s_INDENT.c_str());
	m_physical_Network.to_Screen(indent + s2_INDENT);

	fprintf(fw, "%s%svirtual networks: {\n", indent.c_str(), s_INDENT.c_str());	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_virtual_Networks[vn_id].to_Screen(indent + s2_INDENT);
	}
	fprintf(fw, "%s%s}\n", indent.c_str(), s_INDENT.c_str());		

	fprintf(fw, "%s%smapping offsets: {\n", indent.c_str(), s_INDENT.c_str());		
	for (Offsets_vector::const_iterator mapping_offset = m_mapping_Offsets.begin(); mapping_offset != m_mapping_Offsets.end(); ++mapping_offset)
	{
	    fprintf(fw, "%s%s%d\n", indent.c_str(), s2_INDENT.c_str(), *mapping_offset);
	}
	fprintf(fw, "%s%s}\n", indent.c_str(), s_INDENT.c_str());
	fprintf(fw, "%s}\n", indent.c_str());	
    }



    
/*============================================================================*/
// sPathEmbeddingModel class
/*----------------------------------------------------------------------------*/

    Glucose::Solver* sPathEmbeddingModel::setup_SATSolver(sDouble timeout)
    {
	Glucose::Solver *solver;
	solver = new Glucose::Solver;

	solver->s_Glucose_timeout = timeout;
	solver->setIncrementalMode();

	return solver;	
    }


    void sPathEmbeddingModel::destroy_SATSolver(Glucose::Solver *solver)
    {
	delete solver;
    }

    
    void sPathEmbeddingModel::setup_LazyPathModel(sBoolEncoder *encoder, sDouble geographical_distance)
    {
	sInt_32 offset = 1;
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_vertex_mapping_Offsets.push_back(offset);
	    offset += m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();
	}
	m_last_vertex_mapping_variable = offset;

	m_edge_mapping_Offsets.resize(m_virtual_Networks.size());
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_edge_mapping_Offsets[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    m_edge_mapping_Offsets[vn_id][virt_v_id].push_back(offset);
		    offset += m_physical_Network.get_VertexCount() * m_physical_Network.get_VertexCount();
		}
	    }
	}
	m_last_edge_mapping_variable = offset;
	encoder->set_LastVariableID(offset);

	m_geographical_distance = geographical_distance;
    }


    void sPathEmbeddingModel::setup_LimitedLazyPathModel(sBoolEncoder *encoder, sInt_32 depth, sDouble geographical_distance)
    {
	sInt_32 offset = 1;
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_vertex_mapping_Offsets.push_back(offset);
	    offset += m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();
	}
	m_last_vertex_mapping_variable = offset;

	m_edge_mapping_Offsets.resize(m_virtual_Networks.size());
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_edge_mapping_Offsets[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    m_edge_mapping_Offsets[vn_id][virt_v_id].push_back(offset);
		    offset += m_physical_Network.get_VertexCount() * depth;
		}
	    }
	}
	m_last_edge_mapping_variable = offset;
	encoder->set_LastVariableID(offset);

	m_geographical_distance = geographical_distance;	
    }    


    void sPathEmbeddingModel::setup_LazyTreeModel(sBoolEncoder *encoder, sDouble geographical_distance)
    {
	sInt_32 offset = 1;
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_vertex_mapping_Offsets.push_back(offset);
	    offset += m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();
	}
	m_last_vertex_mapping_variable = offset;

	m_edge_mapping_Offsets.resize(m_virtual_Networks.size());
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_edge_mapping_Offsets[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    m_edge_mapping_Offsets[vn_id][virt_v_id].push_back(offset);
		    offset += m_physical_Network.get_VertexCount() * m_physical_Network.get_VertexCount();
		}
	    }
	}
	m_last_edge_mapping_variable = offset;
	encoder->set_LastVariableID(offset);

	m_geographical_distance = geographical_distance;
    }


    void sPathEmbeddingModel::setup_LimitedLazyTreeModel(sBoolEncoder *encoder, sInt_32 depth, sDouble geographical_distance)
    {
	sInt_32 offset = 1;
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_vertex_mapping_Offsets.push_back(offset);
	    offset += m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();
	}
	m_last_vertex_mapping_variable = offset;

	m_edge_mapping_Offsets.resize(m_virtual_Networks.size());
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_edge_mapping_Offsets[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    m_edge_mapping_Offsets[vn_id][virt_v_id].push_back(offset);
		    offset += m_physical_Network.get_VertexCount() * depth;
		}
	    }
	}
	m_last_edge_mapping_variable = offset;
	encoder->set_LastVariableID(offset);

	m_geographical_distance = geographical_distance;	
    }


    void sPathEmbeddingModel::setup_LazyGraphModel(sBoolEncoder *encoder, sDouble geographical_distance)
    {
	sInt_32 offset = 1;
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_vertex_mapping_Offsets.push_back(offset);
	    offset += m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();
	}
	m_last_vertex_mapping_variable = offset;

	m_edge_mapping_Offsets.resize(m_virtual_Networks.size());
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_edge_mapping_Offsets[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    m_edge_mapping_Offsets[vn_id][virt_v_id].push_back(offset);
		    offset += m_physical_Network.get_VertexCount() * m_physical_Network.get_VertexCount();
		}
	    }
	}
	m_last_edge_mapping_variable = offset;
	encoder->set_LastVariableID(offset);

	m_geographical_distance = geographical_distance;
    }


    void sPathEmbeddingModel::setup_LimitedLazyGraphModel(sBoolEncoder *encoder, sInt_32 depth, sDouble geographical_distance)
    {
	sInt_32 offset = 1;
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_vertex_mapping_Offsets.push_back(offset);
	    offset += m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();
	}
	m_last_vertex_mapping_variable = offset;

	m_edge_mapping_Offsets.resize(m_virtual_Networks.size());
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_edge_mapping_Offsets[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    m_edge_mapping_Offsets[vn_id][virt_v_id].push_back(offset);
		    offset += m_physical_Network.get_VertexCount() * depth;
		}
	    }
	}
	m_last_edge_mapping_variable = offset;
	encoder->set_LastVariableID(offset);

	m_geographical_distance = geographical_distance;	
    }


    void sPathEmbeddingModel::setup_LazyFlatModel(sBoolEncoder *encoder, sDouble geographical_distance)
    {
	sInt_32 offset = 1;

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_vertex_mapping_Offsets.push_back(offset);
	    offset += m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();
	}
	m_last_vertex_mapping_variable = offset;

	sInt_32 physical_vertex_count = m_physical_Network.get_VertexCount();
	//sInt_32 physical_edge_count = m_physical_Network.get_OutEdgeCount();

	m_edge_mapping_Offsets.resize(m_virtual_Networks.size());
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_edge_mapping_Offsets[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());	    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    m_edge_mapping_Offsets[vn_id][virt_v_id].push_back(offset);
		    offset += physical_vertex_count;
		}
	    }
	}

	m_last_edge_mapping_variable = offset;	

	m_edge_neighbor_mapping_Offsets.resize(m_virtual_Networks.size());	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_edge_neighbor_mapping_Offsets[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());	    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		m_edge_neighbor_mapping_Offsets[vn_id][virt_v_id].resize(m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.size());
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    //m_edge_neighbor_mapping_Offsets[vn_id][virt_v_id][neighbor_index].resize(m_physical_Network.get_VertexCount());
		    
		    for (sInt_32 phys_vertex_id = 0; phys_vertex_id < m_physical_Network.get_VertexCount(); ++phys_vertex_id)
		    {		    
			m_edge_neighbor_mapping_Offsets[vn_id][virt_v_id][neighbor_index].push_back(offset);
			offset += m_physical_Network.m_Vertices[phys_vertex_id].m_out_Neighbors.size();
		    }
		    ++neighbor_index;
		}
	    }
	}

	m_last_edge_neighbor_mapping_variable = offset;
	encoder->set_LastVariableID(offset);

	m_geographical_distance = geographical_distance;
    }


/*----------------------------------------------------------------------------*/
  
    void sPathEmbeddingModel::build_LazyPathModel(sBoolEncoder *encoder, Glucose::Solver *solver)
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_u_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);		    
			sInt_32 start_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, 0, phys_u_id);
			encoder->cast_Implication(solver, vertex_u_mapping_ID, start_edge_mapping_ID);
		    }

		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_u_id);
			sBoolEncoder::VariableIDs_vector end_edge_mapping_IDs;

			for (sInt_32 phys_index = 1; phys_index < m_physical_Network.get_VertexCount(); ++phys_index)
			{
//			    calc_EdgeEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_u_id, sInt_32 phys_v_id) const			    
				sInt_32 end_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, phys_index, phys_u_id);
//			    sInt_32 end_edge_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_v_id);
			    end_edge_mapping_IDs.push_back(end_edge_mapping_ID);
			}
			encoder->cast_MultiDisjunctiveImplication(solver, vertex_v_mapping_ID, end_edge_mapping_IDs);
		    }
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		    
		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    
		    build_IndividualPathModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index);
		    ++neighbor_index;
		}		    
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
		encoder->cast_Disjunction(solver, mapping_IDs);
	    }
	}
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
	    }
	}

	/*
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 neighbor_index = 0;
		    for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		    {
			sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

			for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
			{
			    if (phys_u_id != phys_v_id)
			    {
				build_IndividualCorrespondence(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, phys_u_id, phys_v_id);
			    }
			}
			++neighbor_index;
		    }		    
		}
	    }
	}
	*/

	if (m_geographical_distance >= 0.0)
	{
	    build_GeographicalConstraints(encoder, solver);
	}
       
    }


    void sPathEmbeddingModel::build_LimitedLazyPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 depth, GeoCircles_vector *geo_Circles)
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_u_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);		    
			sInt_32 start_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, 0, phys_u_id);
			encoder->cast_Implication(solver, vertex_u_mapping_ID, start_edge_mapping_ID);
		    }

		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_u_id);
			sBoolEncoder::VariableIDs_vector end_edge_mapping_IDs;

			for (sInt_32 phys_index = 1; phys_index < depth; ++phys_index)
			{
//			    calc_EdgeEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_u_id, sInt_32 phys_v_id) const			    
			    sInt_32 end_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, phys_index, phys_u_id);
//			    sInt_32 end_edge_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_v_id);
			    end_edge_mapping_IDs.push_back(end_edge_mapping_ID);
			}
			encoder->cast_MultiDisjunctiveImplication(solver, vertex_v_mapping_ID, end_edge_mapping_IDs);
		    }
		    ++neighbor_index;
		}
	    }
	}
	clock_t time_begin, time_finish;
	time_begin = clock();
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		    
		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

		    double rnd = (double)rand() / RAND_MAX;
		    if (rnd < 0.65)
		    {
			build_LimitedIndividualPathModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, depth);
		    }
		    else
		    {
			build_SublimitedIndividualPathModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, depth);			
		    }
		    ++neighbor_index;
		}		    
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
		encoder->cast_Disjunction(solver, mapping_IDs);
	    }
	}
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
	    }
	}
	time_finish = clock();
	printf("Intermediate time: %.3f\n", (time_finish - time_begin) / (sDouble)CLOCKS_PER_SEC);	

	/*
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 neighbor_index = 0;
		    for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		    {
			sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

			for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
			{
			    if (phys_u_id != phys_v_id)
			    {
				build_IndividualCorrespondence(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, phys_u_id, phys_v_id);
			    }
			}
			++neighbor_index;
		    }		    
		}
	    }
	}
	*/
	if (geo_Circles != NULL)
	{
	    collect_GeographicalConstraints(geo_Circles);	    
	}
	else
	{	
	    if (m_geographical_distance >= 0.0)
	    {
		build_GeographicalConstraints(encoder, solver);
	    }
	}
    }


    void sPathEmbeddingModel::build_LazyTreeModel(sBoolEncoder *encoder, Glucose::Solver *solver)
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_u_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);		    
			sInt_32 start_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, 0, phys_u_id);
			encoder->cast_Implication(solver, vertex_u_mapping_ID, start_edge_mapping_ID);
		    }

		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_u_id);
			sBoolEncoder::VariableIDs_vector end_edge_mapping_IDs;

			for (sInt_32 phys_index = 1; phys_index < m_physical_Network.get_VertexCount(); ++phys_index)
			{
//			    calc_EdgeEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_u_id, sInt_32 phys_v_id) const			    
				sInt_32 end_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, phys_index, phys_u_id);
//			    sInt_32 end_edge_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_v_id);
			    end_edge_mapping_IDs.push_back(end_edge_mapping_ID);
			}
			encoder->cast_MultiDisjunctiveImplication(solver, vertex_v_mapping_ID, end_edge_mapping_IDs);
		    }
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		    
		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    build_IndividualTreeModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index);
		       
		    ++neighbor_index;
		}		    
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
		encoder->cast_Disjunction(solver, mapping_IDs);
	    }
	}
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
	    }
	}

	/*
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 neighbor_index = 0;
		    for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		    {
			sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

			for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
			{
			    if (phys_u_id != phys_v_id)
			    {
				build_IndividualCorrespondence(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, phys_u_id, phys_v_id);
			    }
			}
			++neighbor_index;
		    }		    
		}
	    }
	}
	*/

	if (m_geographical_distance >= 0.0)
	{
	    build_GeographicalConstraints(encoder, solver);
	}
    }


    void sPathEmbeddingModel::build_LazyFlatModel(sBoolEncoder *encoder, Glucose::Solver *solver)
    {
	printf("alpha 1\n");
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_u_mapping_ID = calc_FlatVertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
			sInt_32 start_edge_mapping_ID = calc_FlatEdgeVertexEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, phys_u_id);
			encoder->cast_Implication(solver, vertex_u_mapping_ID, start_edge_mapping_ID);
		    }

		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_u_id);			
			sInt_32 end_edge_mapping_ID = calc_FlatEdgeVertexEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, phys_u_id);
			encoder->cast_Implication(solver, vertex_v_mapping_ID, end_edge_mapping_ID);
		    }
		    ++neighbor_index;
		}
	    }
	}
	printf("alpha 2\n");	
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		    
		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    
		    build_IndividualFlatModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index);
		    ++neighbor_index;
		}		    
	    }
	}
	printf("alpha 3\n");	

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 path_v_id = calc_FlatVertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
		encoder->cast_Disjunction(solver, mapping_IDs);
	    }
	}
	printf("alpha 4\n");
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
		{
		    sInt_32 path_v_id = calc_FlatVertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
	    }
	}
	printf("alpha 5\n");
	/*
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 neighbor_index = 0;
		    for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		    {
			sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

			for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
			{
			    if (phys_u_id != phys_v_id)
			    {
				build_IndividualCorrespondence(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, phys_u_id, phys_v_id);
			    }
			}
			++neighbor_index;
		    }		    
		}
	    }
	}
	*/

	if (m_geographical_distance >= 0.0)
	{
	    build_GeographicalConstraints(encoder, solver);
	}
	printf("alpha 6\n");
    }    


    void sPathEmbeddingModel::build_LimitedLazyTreeModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 depth, GeoCircles_vector *geo_Circles)
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_u_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);		    
			sInt_32 start_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, 0, phys_u_id);
			encoder->cast_Implication(solver, vertex_u_mapping_ID, start_edge_mapping_ID);
		    }

		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_u_id);
			sBoolEncoder::VariableIDs_vector end_edge_mapping_IDs;

			for (sInt_32 phys_index = 1; phys_index < depth; ++phys_index)			
			{
//			    calc_EdgeEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_u_id, sInt_32 phys_v_id) const			    
				sInt_32 end_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, phys_index, phys_u_id);
//			    sInt_32 end_edge_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_v_id);
			    end_edge_mapping_IDs.push_back(end_edge_mapping_ID);
			}
			encoder->cast_MultiDisjunctiveImplication(solver, vertex_v_mapping_ID, end_edge_mapping_IDs);
		    }
		    ++neighbor_index;
		}
	    }
	}
	clock_t time_begin, time_finish;
	time_begin = clock();
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		    
		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

		    double rnd = (double)rand() / RAND_MAX;
		    if (rnd < 0.65)
		    {
			build_LimitedIndividualTreeModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, depth);
		    }
		    else
		    {
			build_SublimitedIndividualTreeModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, depth);
		    }
		    ++neighbor_index;
		}		    
	    }
	}
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
		encoder->cast_Disjunction(solver, mapping_IDs);
	    }
	}
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
	    }
	}
	time_finish = clock();
	printf("Intermediate time: %.3f\n", (time_finish - time_begin) / (sDouble)CLOCKS_PER_SEC);

	/*
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 neighbor_index = 0;
		    for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		    {
			sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

			for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
			{
			    if (phys_u_id != phys_v_id)
			    {
				build_IndividualCorrespondence(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, phys_u_id, phys_v_id);
			    }
			}
			++neighbor_index;
		    }		    
		}
	    }
	}
	*/
	if (geo_Circles != NULL)
	{
	    collect_GeographicalConstraints(geo_Circles);	    
	}
	else
	{		
	    if (m_geographical_distance >= 0.0)
	    {
		build_GeographicalConstraints(encoder, solver);
	    }
	}
    }    

    
    void sPathEmbeddingModel::build_LazyGraphModel(sBoolEncoder *encoder, Glucose::Solver *solver)
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_u_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);		    
			sInt_32 start_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, 0, phys_u_id);
			encoder->cast_Implication(solver, vertex_u_mapping_ID, start_edge_mapping_ID);
		    }

		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_u_id);
			sBoolEncoder::VariableIDs_vector end_edge_mapping_IDs;

			for (sInt_32 phys_index = 1; phys_index < m_physical_Network.get_VertexCount(); ++phys_index)
			{
//			    calc_EdgeEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_u_id, sInt_32 phys_v_id) const			    
				sInt_32 end_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, phys_index, phys_u_id);
//			    sInt_32 end_edge_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_v_id);
			    end_edge_mapping_IDs.push_back(end_edge_mapping_ID);
			}
			encoder->cast_MultiDisjunctiveImplication(solver, vertex_v_mapping_ID, end_edge_mapping_IDs);
		    }
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		    
		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    
		    build_IndividualGraphModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index);
		    ++neighbor_index;
		}		    
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
		encoder->cast_Disjunction(solver, mapping_IDs);
	    }
	}
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
	    }
	}

	/*
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 neighbor_index = 0;
		    for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		    {
			sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

			for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
			{
			    if (phys_u_id != phys_v_id)
			    {
				build_IndividualCorrespondence(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, phys_u_id, phys_v_id);
			    }
			}
			++neighbor_index;
		    }		    
		}
	    }
	}
	*/

	if (m_geographical_distance >= 0.0)
	{
	    build_GeographicalConstraints(encoder, solver);
	}
    }


    void sPathEmbeddingModel::build_LimitedLazyGraphModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 depth, GeoCircles_vector *geo_Circles)
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_u_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);		    
			sInt_32 start_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, 0, phys_u_id);
			encoder->cast_Implication(solver, vertex_u_mapping_ID, start_edge_mapping_ID);
		    }

		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_u_id);
			sBoolEncoder::VariableIDs_vector end_edge_mapping_IDs;

			for (sInt_32 phys_index = 1; phys_index < depth; ++phys_index)
			{
//			    calc_EdgeEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_u_id, sInt_32 phys_v_id) const			    
				sInt_32 end_edge_mapping_ID = calc_EdgeEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, phys_index, phys_u_id);
//			    sInt_32 end_edge_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_v_id);
			    end_edge_mapping_IDs.push_back(end_edge_mapping_ID);
			}
			encoder->cast_MultiDisjunctiveImplication(solver, vertex_v_mapping_ID, end_edge_mapping_IDs);
		    }
		    ++neighbor_index;
		}
	    }
	}
	clock_t time_begin, time_finish;
	time_begin = clock();
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		    
		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

		    double rnd = (double)rand() / RAND_MAX;
		    if (rnd < 0.65)
		    {		    
			build_LimitedIndividualGraphModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, depth);
		    }
		    else
		    {
			build_SublimitedIndividualGraphModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, depth);			
		    }
		    ++neighbor_index;
		}		    
	    }
	}
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
		encoder->cast_Disjunction(solver, mapping_IDs);
	    }
	}
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
		{
		    sInt_32 path_v_id = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
	    }
	}
	time_finish = clock();
	printf("Intermediate time: %.3f\n", (time_finish - time_begin) / (sDouble)CLOCKS_PER_SEC);

	/*
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 neighbor_index = 0;
		    for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		    {
			sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

			for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
			{
			    if (phys_u_id != phys_v_id)
			    {
				build_IndividualCorrespondence(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, phys_u_id, phys_v_id);
			    }
			}
			++neighbor_index;
		    }		    
		}
	    }
	}
	*/
	if (geo_Circles != NULL)
	{
	    collect_GeographicalConstraints(geo_Circles);
	}
	else
	{		
	    if (m_geographical_distance >= 0.0)
	    {
		build_GeographicalConstraints(encoder, solver);
	    }
	}
    }


    void sPathEmbeddingModel::build_LazyFlatModel(sBoolEncoder *encoder, Glucose::Solver *solver, GeoCircles_vector *geo_Circles)
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_u_mapping_ID = calc_FlatVertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);		    
			sInt_32 start_edge_mapping_ID = calc_FlatEdgeVertexEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, phys_u_id);
			encoder->cast_Implication(solver, vertex_u_mapping_ID, start_edge_mapping_ID);
		    }

		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			sInt_32 vertex_v_mapping_ID = calc_FlatVertexEmbeddingBitVariableID(vn_id, virt_neighbor_id, phys_u_id);
			sInt_32 end_edge_mapping_ID = calc_FlatEdgeVertexEmbeddingBitVariableID(vn_id, virt_u_id, neighbor_index, phys_u_id);
			encoder->cast_Implication(solver, vertex_v_mapping_ID, end_edge_mapping_ID);
		    }
		    ++neighbor_index;
		}
	    }
	}
	clock_t time_begin, time_finish;
	time_begin = clock();
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		    
		    sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

		    build_IndividualFlatModel(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index);
		    ++neighbor_index;
		}		    
	    }
	}
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 path_v_id = calc_FlatVertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
		encoder->cast_Disjunction(solver, mapping_IDs);
	    }
	}
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		sBoolEncoder::VariableIDs_vector mapping_IDs;
		for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
		{
		    sInt_32 path_v_id = calc_FlatVertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
		    mapping_IDs.push_back(path_v_id);		    
		}
		encoder->cast_AdaptiveAllMutexConstraint(solver, mapping_IDs);
	    }
	}
	time_finish = clock();
	printf("Intermediate time: %.3f\n", (time_finish - time_begin) / (sDouble)CLOCKS_PER_SEC);

	/*
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    sInt_32 neighbor_index = 0;
		    for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		    {
			sInt_32 virt_neighbor_id = (*virt_neighbor)->m_target->m_id;

			for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
			{
			    if (phys_u_id != phys_v_id)
			    {
				build_IndividualCorrespondence(encoder, solver, vn_id, virt_u_id, virt_neighbor_id, neighbor_index, phys_u_id, phys_v_id);
			    }
			}
			++neighbor_index;
		    }		    
		}
	    }
	}
	*/	
	if (geo_Circles != NULL)
	{
	    collect_GeographicalConstraints(geo_Circles);
	}
	else
	{		
	    if (m_geographical_distance >= 0.0)
	    {
		build_GeographicalConstraints(encoder, solver);
	    }
	}
    }    

    
    void sPathEmbeddingModel::build_IndividualPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index)
    {
	for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		//sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
		++path_v_id;
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, column_IDs);	    
	}

	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);	    
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
		path_u_id += m_physical_Network.get_VertexCount();
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}

	sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, 0);	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    //sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;

	    sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, 0);
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;
                //calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	    ++path_u_id;
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < m_physical_Network.get_VertexCount() - 1; ++phys_u_id)
	{
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);
	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);	    
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;

		sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, 0);
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;
			// calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		//sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
		++path_u_id;
		++vertex_v_mapping_ID;
	    }
	}
	path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, m_physical_Network.get_VertexCount() - 1, 0);
	sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    //sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, m_physical_Network.get_VertexCount() - 1, phys_v_id);

	    // sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);
	    ++path_u_id;
	    ++vertex_v_mapping_ID;
	}	

/*
	for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, column_IDs);	    
	}

	for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);		
	    }
	}
	*/
    }

/*
    void sPathEmbeddingModel::build_LimitedIndividualPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index, sInt_32 depth)
    {
	sASSERT(depth >= 2 && depth < m_physical_Network.get_VertexCount());

	for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);	    
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		//sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
		++path_v_id;
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, column_IDs);
	}

	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);	    
	    for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	    {
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
		path_u_id += m_physical_Network.get_VertexCount();		
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}

	sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, 0);		
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    //sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;

	    sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, 0);	    
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;		
		//sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	    ++path_u_id;
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);
	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);	    
	    
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;

		sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, 0);
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;		    
		    //sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
		++path_u_id;
	    }
	}

	path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, m_physical_Network.get_VertexCount() - 1, 0);
	sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);		
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
//	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);
//	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);	    
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);
	    ++path_u_id;
	    ++vertex_v_mapping_ID;

	}
    }
*/
    

    void sPathEmbeddingModel::build_LimitedIndividualPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index, sInt_32 depth)
    {
	sASSERT(depth >= 2 && depth < m_physical_Network.get_VertexCount());

	for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
	    }
	    encoder->cast_ProductAllMutexConstraint(solver, column_IDs);
	}

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	    {
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}       	

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;
								    
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	}
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);

	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);		    
	}
    }


    void sPathEmbeddingModel::build_SublimitedIndividualPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index, sInt_32 depth)
    {
	sASSERT(depth >= 2 && depth < m_physical_Network.get_VertexCount());

	for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
	    }
	    encoder->cast_ProductAllMutexConstraint(solver, column_IDs);
	}

	sBoolEncoder::VariableIDs_vector column_IDs;
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);
	    encoder->cast_BitUnset(solver, path_v_id);	    
	}	

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	    {
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}       	

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;
								    
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	}
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);

	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);		    
	}
    }    


    void sPathEmbeddingModel::build_IndividualTreeModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index)
    {	
	for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		//sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
		++path_v_id;
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, column_IDs);	    
	}

/*	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);	    
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
		path_u_id += m_physical_Network.get_VertexCount();
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}
*/
	sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, 0);	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    //sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;

	    sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, 0);
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;
                //calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	    ++path_u_id;
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < m_physical_Network.get_VertexCount() - 1; ++phys_u_id)
	{
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);
	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);	    
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;

		sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, 0);
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;
			// calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		//sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
		++path_u_id;
		++vertex_v_mapping_ID;
	    }
	}
	path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, m_physical_Network.get_VertexCount() - 1, 0);
	sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    //sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, m_physical_Network.get_VertexCount() - 1, phys_v_id);

	    // sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);
	    ++path_u_id;
	    ++vertex_v_mapping_ID;
	}	

/*
	for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, column_IDs);	    
	}

	for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);		
	    }
	}
	*/
    }

/*
    void sPathEmbeddingModel::build_LimitedIndividualTreeModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index, sInt_32 depth)
    {
	sASSERT(depth >= 2 && depth < m_physical_Network.get_VertexCount());

	for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);	    
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		//sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
		++path_v_id;
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, column_IDs);
	}

	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);	    
	    for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	    {
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
		path_u_id += m_physical_Network.get_VertexCount();		
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}

	sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, 0);		
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    //sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;

	    sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, 0);	    
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;		
		//sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	    ++path_u_id;
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);
	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);	    
	    
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;

		sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, 0);
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;		    
		    //sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
		++path_u_id;
	    }
	}

	path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, m_physical_Network.get_VertexCount() - 1, 0);
	sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);		
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
//	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);
//	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);	    
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);
	    ++path_u_id;
	    ++vertex_v_mapping_ID;

	}
    }
*/
    

    void sPathEmbeddingModel::build_LimitedIndividualTreeModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index, sInt_32 depth)
    {
	sASSERT(depth >= 2 && depth < m_physical_Network.get_VertexCount());

	for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
	    }
	    encoder->cast_ProductAllMutexConstraint(solver, column_IDs);
	}

/*
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	    {
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}       	
*/	    
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;
								    
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	}

/*
	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id - 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	}	
*/
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);

	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);		    
	}
    }


    void sPathEmbeddingModel::build_SublimitedIndividualTreeModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index, sInt_32 depth)
    {
	sASSERT(depth >= 2 && depth < m_physical_Network.get_VertexCount());

	for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
	    }
	    encoder->cast_ProductAllMutexConstraint(solver, column_IDs);
	}

/*
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	    {
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}       	
*/

	sBoolEncoder::VariableIDs_vector column_IDs;
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);
	    encoder->cast_BitUnset(solver, path_v_id);	    
	}	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;
								    
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	}

/*
	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id - 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	}	
*/
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);

	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);		    
	}
    }

    
    void sPathEmbeddingModel::build_IndividualGraphModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index)
    {
/*
	for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		//sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
		++path_v_id;
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, column_IDs);	    
	}

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);	    
	    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	    {
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
		path_u_id += m_physical_Network.get_VertexCount();
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}
*/
	sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, 0);	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    //sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;

	    sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, 0);
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;
                //calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	    ++path_u_id;
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < m_physical_Network.get_VertexCount() - 1; ++phys_u_id)
	{
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);
	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);	    
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;

		sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, 0);
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;
			// calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		//sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
		++path_u_id;
		++vertex_v_mapping_ID;
	    }
	}
	path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, m_physical_Network.get_VertexCount() - 1, 0);
	sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    //sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, m_physical_Network.get_VertexCount() - 1, phys_v_id);

	    // sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);
	    ++path_u_id;
	    ++vertex_v_mapping_ID;
	}	

/*
	for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, column_IDs);	    
	}

	for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);		
	    }
	}
	*/
    }

/*
    void sPathEmbeddingModel::build_LimitedIndividualGraphModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index, sInt_32 depth)
    {
	sASSERT(depth >= 2 && depth < m_physical_Network.get_VertexCount());

	for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);	    
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		//sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
		++path_v_id;
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, column_IDs);
	}

	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);	    
	    for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	    {
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
		path_u_id += m_physical_Network.get_VertexCount();		
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}

	sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, 0);		
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    //sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;

	    sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, 0);	    
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;		
		//sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	    ++path_u_id;
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, 0);
	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);	    
	    
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		//sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;

		sInt_32 path_v_base_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, 0);
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = path_v_base_id + phys_neighbor_id;		    
		    //sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
		++path_u_id;
	    }
	}

	path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, m_physical_Network.get_VertexCount() - 1, 0);
	sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, 0);		
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
//	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);
//	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);	    
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);
	    ++path_u_id;
	    ++vertex_v_mapping_ID;

	}
    }
*/
    

    void sPathEmbeddingModel::build_LimitedIndividualGraphModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index, sInt_32 depth)
    {
	sASSERT(depth >= 2 && depth < m_physical_Network.get_VertexCount());
/*
	for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
	    }
	    encoder->cast_ProductAllMutexConstraint(solver, column_IDs);
	}

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	    {
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}       	
*/	    
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;
								    
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	}

/*
	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id - 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	}	
*/
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);

	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);		    
	}
    }


    void sPathEmbeddingModel::build_SublimitedIndividualGraphModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index, sInt_32 depth)
    {
	sASSERT(depth >= 2 && depth < m_physical_Network.get_VertexCount());
/*
	for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	{
	    sBoolEncoder::VariableIDs_vector column_IDs;
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		column_IDs.push_back(path_v_id);
	    }
	    encoder->cast_ProductAllMutexConstraint(solver, column_IDs);
	}

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector row_IDs;
	    for (sInt_32 phys_u_id = 0; phys_u_id < depth; ++phys_u_id)
	    {
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);
		row_IDs.push_back(path_u_id);
	    }
	    encoder->cast_AdaptiveAllMutexConstraint(solver, row_IDs);	    
	}       	
*/

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);
	    encoder->cast_BitUnset(solver, path_v_id);	    
	}	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_v_id);
	    sBoolEncoder::VariableIDs_vector target_IDs;
								    
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 1, phys_neighbor_id);
		target_IDs.push_back(path_v_id);
	    }	
	    //sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    //encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    
            //target_IDs.push_back(vertex_v_mapping_ID);

	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_u_id);
	    }
	}

	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id + 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);		    

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	}

/*
	for (sInt_32 phys_u_id = 1; phys_u_id < depth - 1; ++phys_u_id)
	{
	    for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	    {		
		sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id, phys_v_id);

		sBoolEncoder::VariableIDs_vector target_IDs;
								    
		for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.end(); ++phys_neighbor)
		{
		    sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_u_id - 1, phys_neighbor_id);
		    target_IDs.push_back(path_v_id);
		}	
		sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
//		encoder->cast_ImpliedMultiImplication(solver, vertex_v_mapping_ID, path_u_id, target_IDs);

		target_IDs.push_back(vertex_v_mapping_ID);		
		encoder->cast_MultiImplication(solver, path_u_id, target_IDs);
	    }
	}	
*/
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{		
	    sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, depth - 1, phys_v_id);

	    sInt_32 vertex_v_mapping_ID = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    encoder->cast_Implication(solver, path_u_id, vertex_v_mapping_ID);		    
	}
    }
        
    
    void sPathEmbeddingModel::build_IndividualFlatModel(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index)
    {
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sBoolEncoder::VariableIDs_vector target_IDs;
	    sInt_32 connector_vertex_v_mapping_ID = calc_FlatVertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);
	    
	    sInt_32 path_v_base_id = calc_FlatEdgeVertexEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0);
	    sInt_32 path_v_id = path_v_base_id + phys_v_id;	    

	    sInt_32 phys_neighbor_index = 0;
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 edge_variable_id = calc_FlatEdgeNeighborEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, phys_v_id, phys_neighbor_index);
		
		sInt_32 phys_neighbor_id = (*phys_neighbor)->m_target->m_id;
		sInt_32 path_neighbor_id = path_v_base_id + phys_neighbor_id;

		encoder->cast_Implication(solver, edge_variable_id, path_neighbor_id);
		
		target_IDs.push_back(edge_variable_id);
		++phys_neighbor_index;
	    }
	    sBoolEncoder::VariableIDs_vector source_IDs;

	    sInt_32 connector_vertex_u_mapping_ID = calc_FlatVertexEmbeddingBitVariableID(vnet_id, virt_u_id, phys_v_id);
	    
	    sInt_32 in_neighbor_index = 0;	    
	    for (s_Vertex::Neighbors_vector::const_iterator in_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.begin(); in_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_in_Neighbors.end(); ++in_neighbor)
	    {
		sInt_32 in_neighbor_id = (*in_neighbor)->m_source->m_id;

		sInt_32 inin_neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator inin_neighbor = m_physical_Network.m_Vertices[in_neighbor_id].m_out_Neighbors.begin(); inin_neighbor != m_physical_Network.m_Vertices[in_neighbor_id].m_out_Neighbors.end(); ++inin_neighbor)
		{
		    sInt_32 inin_neighbor_id = (*inin_neighbor)->m_target->m_id;
		    
		    if (inin_neighbor_id == phys_v_id)
		    {
			sInt_32 source_edge_variable_id = calc_FlatEdgeNeighborEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, in_neighbor_id, inin_neighbor_index);
			source_IDs.push_back(source_edge_variable_id);
		    }
		    ++inin_neighbor_index;
		}
		++in_neighbor_index;
	    }

	    source_IDs.push_back(connector_vertex_u_mapping_ID);
	    target_IDs.push_back(connector_vertex_v_mapping_ID);
	    
	    if (!source_IDs.empty())
	    {
		encoder->cast_AdaptiveAllMutexConstraint(solver, source_IDs);
	    }
	    if (!target_IDs.empty())
	    {
		encoder->cast_MultiImplication(solver, path_v_id, target_IDs);
	    }
	    else
	    {
		encoder->cast_BitUnset(solver, path_v_id);
	    }
	}
    }    

    
    void sPathEmbeddingModel::build_IndividualCorrespondence(sBoolEncoder *encoder, Glucose::Solver *solver, sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 virt_v_id, sInt_32 neighbor_index, sInt_32 phys_u_id, sInt_32 phys_v_id)
    {
	sInt_32 virt_u_to_phys_id = calc_VertexEmbeddingBitVariableID(vnet_id, virt_u_id, phys_u_id);
	sInt_32 virt_v_to_phys_id = calc_VertexEmbeddingBitVariableID(vnet_id, virt_v_id, phys_v_id);

	sInt_32 path_u_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, 0, phys_u_id);

	sBoolEncoder::VariableIDs_vector target_v_IDs;
	
	for (sInt_32 ph_u_id = 0; ph_u_id < m_physical_Network.get_VertexCount(); ++ph_u_id)
	{
	    sInt_32 path_v_id = calc_EdgeEmbeddingBitVariableID(vnet_id, virt_u_id, neighbor_index, ph_u_id, phys_v_id);
	    target_v_IDs.push_back(path_v_id);
	}
	encoder->cast_Implication(solver, virt_u_to_phys_id, path_u_id);
	encoder->cast_MultiImplication(solver, virt_v_to_phys_id, target_v_IDs);
    }


    void sPathEmbeddingModel::build_GeographicalConstraints(sBoolEncoder *encoder, Glucose::Solver *solver)
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		{
		    if (!m_physical_Network.m_waxman_Nodes.empty() && !m_virtual_Networks[vn_id].m_waxman_Nodes.empty())
		    {
			sDouble x1 = m_physical_Network.m_waxman_Nodes[phys_u_id].m_x;
			sDouble y1 = m_physical_Network.m_waxman_Nodes[phys_u_id].m_y;

			sDouble x2 = m_virtual_Networks[vn_id].m_waxman_Nodes[virt_u_id].m_x;
			sDouble y2 = m_virtual_Networks[vn_id].m_waxman_Nodes[virt_u_id].m_y;

			sDouble dx = x2 - x1;
			sDouble dy = y2 - y1;

			sDouble distance = sqrt(dx * dx + dy * dy);
		    
			if (distance > m_geographical_distance)
			{
			    sInt_32 vertex_u_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
			    encoder->cast_BitUnset(solver, vertex_u_mapping_ID);
			}
		    }
		}
	    }
	}
    }
		

    void sPathEmbeddingModel::collect_GeographicalConstraints(GeoCircles_vector *geo_Circles)
    {
	for (sDouble circ_radius = 1; circ_radius <= m_geographical_distance; ++circ_radius)
	{
	    geo_Circles->push_back(GeoCircle_vector());
	    GeoCircle_vector &geo_Circle = geo_Circles->back();
	    
	    for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	    {
		for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
		{
		    for (sInt_32 phys_u_id = 0; phys_u_id < m_physical_Network.get_VertexCount(); ++phys_u_id)
		    {
			if (!m_physical_Network.m_waxman_Nodes.empty() && !m_virtual_Networks[vn_id].m_waxman_Nodes.empty())
			{
			    sDouble x1 = m_physical_Network.m_waxman_Nodes[phys_u_id].m_x;
			    sDouble y1 = m_physical_Network.m_waxman_Nodes[phys_u_id].m_y;
			    
			    sDouble x2 = m_virtual_Networks[vn_id].m_waxman_Nodes[virt_u_id].m_x;
			    sDouble y2 = m_virtual_Networks[vn_id].m_waxman_Nodes[virt_u_id].m_y;
			    
			    sDouble dx = x2 - x1;
			    sDouble dy = y2 - y1;
			    
			    sDouble distance = sqrt(dx * dx + dy * dy);
		    
			    if (distance > m_geographical_distance)
			    {
				sInt_32 vertex_u_mapping_ID = calc_VertexEmbeddingBitVariableID(vn_id, virt_u_id, phys_u_id);
				geo_Circle.push_back(vertex_u_mapping_ID);
			    }
			}
		    }
		}
	    }
	}
    }
    
    
    bool sPathEmbeddingModel::solve_LazyPathModel(Glucose::Solver *solver)
    {
	if (!solver->simplify())
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return false;
	}
	Glucose::lbool result = solver->solve_();

	if (result == l_True)
	{
    	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_satisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return true;
	}
	else if (result == l_False)
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif	    	    
	    return false;
	}
	else if (result == l_Undef)
	{
	    return false;
	}
	else
	{
	    sASSERT(false);
	}
	
	return true;	
    }


    bool sPathEmbeddingModel::solve_LazyPathModel(Glucose::Solver *solver, Glucose::vec<Glucose::Lit> &geo_circ_Assumption)
    {
	if (!solver->simplify())
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return false;
	}
	Glucose::lbool result = solver->solveLimited(geo_circ_Assumption);

	if (result == l_True)
	{
    	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_satisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return true;
	}
	else if (result == l_False)
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif	    	    
	    return false;
	}
	else if (result == l_Undef)
	{
	    return false;
	}
	else
	{
	    sASSERT(false);
	}
	
	return true;		
    }


    bool sPathEmbeddingModel::solveAll_LazyPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings, sInt_32 depth, GeoCircles_vector *geo_Circles)
    {
	if (geo_Circles != NULL)
	{
	    sDouble geo_distance = 1.0;
	    
	    for (GeoCircles_vector::const_iterator geo_Circle = geo_Circles->begin(); geo_Circle != geo_Circles->end(); ++geo_Circle)
	    {
		printf("Solving at geographical distance = %.3f\n", geo_distance);
		
		while (true)
		{
		    printf("Resolving ...\n");

		    Glucose::vec<Glucose::Lit> geo_circ_Assumption;
		    encoder->build_NegativeAssumption(solver, *geo_Circle, geo_circ_Assumption);
		    
		    if (solve_LazyPathModel(solver, geo_circ_Assumption))
		    {
			vertex_Embeddings.clear();
			path_Embeddings.clear();
		
			printf("Virtual network embedding found !\n");	    	    
			decode_LimitedLazyPathModel(solver, vertex_Embeddings, path_Embeddings, depth);

			if (refine_LimitedLazyPathModel(encoder, solver, vertex_Embeddings, path_Embeddings, depth))
			{
			    printf("Model has been refined ...\n");		
			}
			else
			{
			    return true;
			}
		    }
		    else
		    {
			break;
		    }
		}
		++geo_distance;
	    }
	    
	    return false;	    
	}
	else
	{
	    while (true)
	    {
		printf("Resolving ...\n");
	    
		if (solve_LazyPathModel(solver))
		{
		    vertex_Embeddings.clear();
		    path_Embeddings.clear();
		
		    printf("Virtual network embedding found !\n");	    	    
		    decode_LimitedLazyPathModel(solver, vertex_Embeddings, path_Embeddings, depth);

		    if (refine_LimitedLazyPathModel(encoder, solver, vertex_Embeddings, path_Embeddings, depth))
		    {
			printf("Model has been refined ...\n");		
		    }
		    else
		    {
			return true;
		    }
		}
		else
		{
		    return false;
		}
	    }
	    return false;
	}
    }


    sDouble sPathEmbeddingModel::solveDepthIncreasing_LazyPathModel(sBoolEncoder *encoder, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings, sDouble geographical_distance, sInt_32 max_depth, GeoCircles_vector *geo_Circles)
    {
	for (sInt_32 depth = 3; depth <= max_depth; ++depth)
	{
	    Glucose::Solver *solver = setup_SATSolver();
		
	    setup_LimitedLazyPathModel(encoder, depth, geographical_distance);
	    build_LimitedLazyPathModel(encoder, solver, depth, geo_Circles);
	    printf("Number of Boolean variables: %d\n", encoder->m_last_variable_ID);	    

	    if (solveAll_LazyPathModel(encoder, solver, vertex_Embeddings, path_Embeddings, depth, geo_Circles))
	    {
		to_Screen_embedding(vertex_Embeddings, path_Embeddings);
		sDouble cost = calc_EmbeddingCost(encoder, solver, vertex_Embeddings, path_Embeddings, depth);

		return cost;
	    }

	    destroy_SATSolver(solver);
	    
	    geo_Circles->clear();
	    m_vertex_mapping_Offsets.clear();
	    m_edge_mapping_Offsets.clear();
	}
	
	return -1.0;
    }   


    bool sPathEmbeddingModel::solve_LazyTreeModel(Glucose::Solver *solver)
    {
	if (!solver->simplify())
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return false;
	}
	Glucose::lbool result = solver->solve_();

	if (result == l_True)
	{
    	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_satisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return true;
	}
	else if (result == l_False)
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif	    	    
	    return false;
	}
	else if (result == l_Undef)
	{
	    return false;
	}
	else
	{
	    sASSERT(false);
	}
	
	return true;	
    }


    bool sPathEmbeddingModel::solve_LazyTreeModel(Glucose::Solver *solver, Glucose::vec<Glucose::Lit> &geo_circ_Assumption)
    {
	if (!solver->simplify())
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return false;
	}
	Glucose::lbool result = solver->solveLimited(geo_circ_Assumption);

	if (result == l_True)
	{
    	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_satisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return true;
	}
	else if (result == l_False)
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif	    	    
	    return false;
	}
	else if (result == l_Undef)
	{
	    return false;
	}
	else
	{
	    sASSERT(false);
	}
	
	return true;	
    }
    

    bool sPathEmbeddingModel::solveAll_LazyTreeModel(sBoolEncoder *encoder, Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings, sInt_32 depth, GeoCircles_vector *geo_Circles)
    {
	if (geo_Circles != NULL)
	{
	    sDouble geo_distance = 1.0;
	    
	    for (GeoCircles_vector::const_iterator geo_Circle = geo_Circles->begin(); geo_Circle != geo_Circles->end(); ++geo_Circle)
	    {
		printf("Solving at geographical distance = %.3f\n", geo_distance);
	
		while (true)
		{
		    printf("Resolving ...\n");

		    Glucose::vec<Glucose::Lit> geo_circ_Assumption;
		    encoder->build_NegativeAssumption(solver, *geo_Circle, geo_circ_Assumption);
		    
		    if (solve_LazyTreeModel(solver, geo_circ_Assumption))
		    {
			vertex_Embeddings.clear();
			path_Embeddings.clear();
			
			printf("Virtual network embedding found !\n");	    	    
			decode_LimitedLazyTreeModel(solver, vertex_Embeddings, path_Embeddings, depth);
			
			if (refine_LimitedLazyTreeModel(encoder, solver, vertex_Embeddings, path_Embeddings, depth))
			{
			    printf("Model has been refined ...\n");		
			}
			else
			{
			    return true;
			}
		    }
		    else
		    {
			break;
		    }
		}
		++geo_distance;
	    }
	    return false;
	}
	else
	{
	    while (true)
	    {
		printf("Resolving ...\n");

		if (solve_LazyTreeModel(solver))
		{
		    vertex_Embeddings.clear();
		    path_Embeddings.clear();
		    
		    printf("Virtual network embedding found !\n");	    	    
		    decode_LimitedLazyTreeModel(solver, vertex_Embeddings, path_Embeddings, depth);
		    
		    if (refine_LimitedLazyTreeModel(encoder, solver, vertex_Embeddings, path_Embeddings, depth))
		    {
			printf("Model has been refined ...\n");		
		    }
		    else
		    {
			return true;
		    }
		}
		else
		{
		    return false;
		}	    
	    }
	    return false;
	}
    }


    sDouble sPathEmbeddingModel::solveDepthIncreasing_LazyTreeModel(sBoolEncoder *encoder, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings, sDouble geographical_distance, sInt_32 max_depth, GeoCircles_vector *geo_Circles)
    {
	for (sInt_32 depth = 3; depth <= max_depth; ++depth)
	{
	    Glucose::Solver *solver = setup_SATSolver();
		
	    setup_LimitedLazyTreeModel(encoder, depth, geographical_distance);
	    build_LimitedLazyTreeModel(encoder, solver, depth, geo_Circles);
	    printf("Number of Boolean variables: %d\n", encoder->m_last_variable_ID);	    

	    if (solveAll_LazyTreeModel(encoder, solver, vertex_Embeddings, path_Embeddings, depth, geo_Circles))
	    {
		to_Screen_embedding(vertex_Embeddings, path_Embeddings);
		sDouble cost = calc_EmbeddingCost(encoder, solver, vertex_Embeddings, path_Embeddings, depth);

		return cost;
	    }

	    destroy_SATSolver(solver);
	    
	    geo_Circles->clear();
	    m_vertex_mapping_Offsets.clear();
	    m_edge_mapping_Offsets.clear();
	}
	
	return -1.0;
    }        


    bool sPathEmbeddingModel::solve_LazyGraphModel(Glucose::Solver *solver)
    {
	if (!solver->simplify())
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return false;
	}
	Glucose::lbool result = solver->solve_();

	if (result == l_True)
	{
    	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_satisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return true;
	}
	else if (result == l_False)
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif	    	    
	    return false;
	}
	else if (result == l_Undef)
	{
	    return false;
	}
	else
	{
	    sASSERT(false);
	}
	
	return true;	
    }


    bool sPathEmbeddingModel::solve_LazyGraphModel(Glucose::Solver *solver, Glucose::vec<Glucose::Lit> &geo_circ_Assumption)
    {
	if (!solver->simplify())
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return false;
	}
	Glucose::lbool result = solver->solveLimited(geo_circ_Assumption);

	if (result == l_True)
	{
    	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_satisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return true;
	}
	else if (result == l_False)
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif	    	    
	    return false;
	}
	else if (result == l_Undef)
	{
	    return false;
	}
	else
	{
	    sASSERT(false);
	}
	
	return true;	
    }


    bool sPathEmbeddingModel::solveAll_LazyGraphModel(sBoolEncoder *encoder, Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings, sInt_32 depth, GeoCircles_vector *geo_Circles)
    {
	if (geo_Circles != NULL)
	{
	    sDouble geo_distance = 1.0;
	    
	    for (GeoCircles_vector::const_iterator geo_Circle = geo_Circles->begin(); geo_Circle != geo_Circles->end(); ++geo_Circle)
	    {
		printf("Solving at geographical distance = %.3f\n", geo_distance);

		while (true)
		{
		    printf("Resolving ...\n");

		    Glucose::vec<Glucose::Lit> geo_circ_Assumption;
		    encoder->build_NegativeAssumption(solver, *geo_Circle, geo_circ_Assumption);
	    
		    if (solve_LazyGraphModel(solver, geo_circ_Assumption))
		    {
			vertex_Embeddings.clear();
			path_Embeddings.clear();
			
			NetworkGraphMappings_vector graph_Embeddings;
		
			printf("Virtual network embedding found !\n");

			decode_LimitedLazyGraphModel(solver, vertex_Embeddings, graph_Embeddings, depth);
			transform_LimitedGraph2PathEmbeddings(vertex_Embeddings, graph_Embeddings, path_Embeddings, depth);

			if (refine_LimitedLazyGraphModel(encoder, solver, vertex_Embeddings, path_Embeddings, depth))
			{
			    printf("Model has been refined ...\n");		
			}
			else
			{
			    return true;
			}
		    }
		    else
		    {
			break;
		    }
		}
		++geo_distance;		
	    }
	    return false;
	}	
	else
	{
	    while (true)
	    {
		printf("Resolving ...\n");
		
		if (solve_LazyGraphModel(solver))
		{
		    vertex_Embeddings.clear();
		    path_Embeddings.clear();
		    
		    NetworkGraphMappings_vector graph_Embeddings;
		
		    printf("Virtual network embedding found !\n");
		    decode_LimitedLazyGraphModel(solver, vertex_Embeddings, graph_Embeddings, depth);
		    transform_LimitedGraph2PathEmbeddings(vertex_Embeddings, graph_Embeddings, path_Embeddings, depth);

		    if (refine_LimitedLazyGraphModel(encoder, solver, vertex_Embeddings, path_Embeddings, depth))
		    {
			printf("Model has been refined ...\n");		
		    }
		    else
		    {
			return true;
		    }
		}
		else
		{
		    return false;
		}
	    }
	    return false;
	}
    }


    sDouble sPathEmbeddingModel::solveDepthIncreasing_LazyGraphModel(sBoolEncoder *encoder, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings, sDouble geographical_distance, sInt_32 max_depth, GeoCircles_vector *geo_Circles)
    {
	for (sInt_32 depth = 3; depth <= max_depth; ++depth)
	{
	    Glucose::Solver *solver = setup_SATSolver();
		
	    setup_LimitedLazyGraphModel(encoder, depth, geographical_distance);
	    build_LimitedLazyGraphModel(encoder, solver, depth, geo_Circles);
	    printf("Number of Boolean variables: %d\n", encoder->m_last_variable_ID);

	    if (solveAll_LazyGraphModel(encoder, solver, vertex_Embeddings, path_Embeddings, depth, geo_Circles))
	    {
		to_Screen_embedding(vertex_Embeddings, path_Embeddings);
		sDouble cost = calc_EmbeddingCost(encoder, solver, vertex_Embeddings, path_Embeddings, depth);

		return cost;
	    }

	    destroy_SATSolver(solver);
	    
	    geo_Circles->clear();
	    m_vertex_mapping_Offsets.clear();
	    m_edge_mapping_Offsets.clear();
	}
	
	return -1.0;
    }


    bool sPathEmbeddingModel::solve_LazyFlatModel(Glucose::Solver *solver)
    {
	if (!solver->simplify())
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return false;
	}
	Glucose::lbool result = solver->solve_();

	if (result == l_True)
	{
    	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_satisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return true;
	}
	else if (result == l_False)
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif	    	    
	    return false;
	}
	else if (result == l_Undef)
	{
	    return false;
	}
	else
	{
	    sASSERT(false);
	}
	
	return true;	
    }


    bool sPathEmbeddingModel::solve_LazyFlatModel(Glucose::Solver *solver, Glucose::vec<Glucose::Lit> &geo_circ_Assumption)
    {
	if (!solver->simplify())
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return false;
	}
	Glucose::lbool result = solver->solveLimited(geo_circ_Assumption);

	if (result == l_True)
	{
    	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_satisfiable_SAT_solver_Calls;
	    }
	    #endif
	    return true;
	}
	else if (result == l_False)
	{
  	    #ifdef sSTATISTICS
	    {
		++s_GlobalStatistics.get_CurrentPhase().m_unsatisfiable_SAT_solver_Calls;
	    }
	    #endif	    	    
	    return false;
	}
	else if (result == l_Undef)
	{
	    return false;
	}
	else
	{
	    sASSERT(false);
	}
	
	return true;	
    }


    bool sPathEmbeddingModel::solveAll_LazyFlatModel(sBoolEncoder *encoder, Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings, GeoCircles_vector *geo_Circles)
    {
	if (geo_Circles != NULL)
	{
	    sDouble geo_distance = 1.0;
	    
	    for (GeoCircles_vector::const_iterator geo_Circle = geo_Circles->begin(); geo_Circle != geo_Circles->end(); ++geo_Circle)
	    {
		printf("Solving at geographical distance = %.3f\n", geo_distance);

		while (true)
		{
		    printf("Resolving ...\n");

		    Glucose::vec<Glucose::Lit> geo_circ_Assumption;
		    encoder->build_NegativeAssumption(solver, *geo_Circle, geo_circ_Assumption);
	    
		    if (solve_LazyFlatModel(solver, geo_circ_Assumption))
		    {
			vertex_Embeddings.clear();
			path_Embeddings.clear();
			
			NetworkFlatMappings_vector flat_Embeddings;
			EdgeNetworkFlatMappings_vector flat_edge_Embeddings;
		
			printf("Virtual network embedding found !\n");

			decode_LazyFlatModel(solver, vertex_Embeddings, flat_Embeddings, flat_edge_Embeddings);
			transform_Flat2PathEmbeddings(vertex_Embeddings, flat_Embeddings, flat_edge_Embeddings, path_Embeddings);

			if (refine_LazyFlatModel(encoder, solver, vertex_Embeddings, path_Embeddings))
			{
			    printf("Model has been refined ...\n");		
			}
			else
			{
			    return true;
			}
		    }
		    else
		    {
			break;
		    }
		}
		++geo_distance;		
	    }
	    return false;
	}	
	else
	{
	    while (true)
	    {
		printf("Resolving ...\n");
		
		if (solve_LazyFlatModel(solver))
		{
		    vertex_Embeddings.clear();
		    path_Embeddings.clear();
		    
		    NetworkFlatMappings_vector flat_Embeddings;
		    EdgeNetworkFlatMappings_vector flat_edge_Embeddings;		    
		
		    printf("Virtual network embedding found !\n");
		    decode_LazyFlatModel(solver, vertex_Embeddings, flat_Embeddings, flat_edge_Embeddings);
		    transform_Flat2PathEmbeddings(vertex_Embeddings, flat_Embeddings, flat_edge_Embeddings, path_Embeddings);

		    if (refine_LazyFlatModel(encoder, solver, vertex_Embeddings, path_Embeddings))
		    {
			printf("Model has been refined ...\n");		
		    }
		    else
		    {
			return true;
		    }
		}
		else
		{
		    return false;
		}
	    }
	    return false;
	}
    }    
    
    
    void sPathEmbeddingModel::setup_PhysicalNetwork_online(const s_DirectedGraph &physical_Network)
    {
	m_physical_Network = physical_Network;
    }

    
    void sPathEmbeddingModel::setup_VirtualNetwork_online(const s_DirectedGraph &virtual_Network)
    {
	m_virtual_Networks.push_back(virtual_Network);
    }


    bool sPathEmbeddingModel::solve_LazyPathModel_online(sInt_32 depth, sDouble geographical_distance, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings)
    {		
	sBoolEncoder encoder;
	Glucose::Solver *solver = setup_SATSolver();

	setup_LimitedLazyPathModel(&encoder, depth, geographical_distance);
	build_LimitedLazyPathModel(&encoder, solver, depth);
	printf("Building model ... finished\n");
	
	if (solveAll_LazyPathModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth))
	{	    
	    reset_Model_online();
	    destroy_SATSolver(solver);
	    return true;
	}
	else
	{
	    reset_Model_online();		
	    destroy_SATSolver(solver);
	    return false;	    
	}
    }


    bool sPathEmbeddingModel::solve_LazyTreeModel_online(sInt_32 depth, sDouble geographical_distance, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings)
    {		
	sBoolEncoder encoder;
	Glucose::Solver *solver = setup_SATSolver();

	setup_LimitedLazyTreeModel(&encoder, depth, geographical_distance);
	build_LimitedLazyTreeModel(&encoder, solver, depth);
	printf("Building model ... finished\n");
	
	if (solveAll_LazyTreeModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth))
	{	    
	    reset_Model_online();
	    destroy_SATSolver(solver);
	    return true;
	}
	else
	{
	    reset_Model_online();		
	    destroy_SATSolver(solver);
	    return false;	    
	}
    }


    bool sPathEmbeddingModel::solve_LazyGraphModel_online(sInt_32 depth, sDouble geographical_distance, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings)
    {		
	sBoolEncoder encoder;
	Glucose::Solver *solver = setup_SATSolver();

	setup_LimitedLazyTreeModel(&encoder, depth, geographical_distance);
	build_LimitedLazyTreeModel(&encoder, solver, depth);
	printf("Building model ... finished\n");
	
	if (solveAll_LazyTreeModel(&encoder, solver, vertex_Embeddings, path_Embeddings, depth))
	{	    
	    reset_Model_online();
	    destroy_SATSolver(solver);
	    return true;
	}
	else
	{
	    reset_Model_online();		
	    destroy_SATSolver(solver);
	    return false;	    
	}
    }

   
    void sPathEmbeddingModel::reset_Model_online(void)
    {
	m_vertex_mapping_Offsets.clear();
	m_last_vertex_mapping_variable = 0;
	m_last_edge_mapping_variable = 0;	
	m_edge_mapping_Offsets.clear();

	m_virtual_Networks.clear();		
    }

    
    void sPathEmbeddingModel::decode_LazyPathModel(Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings)
    {
	vertex_Embeddings.resize(m_virtual_Networks.size());
	path_Embeddings.resize(m_virtual_Networks.size());
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    path_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    vertex_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());

	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		path_Embeddings[vn_id][i].resize(m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.size());
		    
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    path_Embeddings[vn_id][i][neighbor_index].resize(m_physical_Network.get_VertexCount(), -1);
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 i = 0; i < m_last_vertex_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 vnet_id, virt_v_id, phys_v_id;		
		sInt_32 variable_ID = sABS(literal);

		decode_VertexEmbeddingMapping(variable_ID, vnet_id, virt_v_id, phys_v_id);

		vertex_Embeddings[vnet_id][virt_v_id] = phys_v_id;
	    }	    
	}
	for (sInt_32 i = m_last_vertex_mapping_variable - 1; i < m_last_edge_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 variable_ID = sABS(literal);
	    
		sInt_32 vnet_id;
		sInt_32 u_id, v_id, neighbor_index;
		sInt_32 phys_u_id = -1, path_index = -1;
		
		decode_EdgeEmbeddingMapping(variable_ID, vnet_id, u_id, v_id, neighbor_index, phys_u_id, path_index);
		sASSERT(phys_u_id != -1 && path_index != -1);

		path_Embeddings[vnet_id][u_id][neighbor_index][path_index] = phys_u_id;
//		printf("vnet_id: %d, u_id: %d, v_id:%d, phys_u_id:%d, path_index:%d\n", vnet_id, u_id, v_id, phys_u_id, path_index);
	    }
	}	
    }


    void sPathEmbeddingModel::decode_LimitedLazyPathModel(Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings, sInt_32 depth)
    {
	vertex_Embeddings.resize(m_virtual_Networks.size());
	path_Embeddings.resize(m_virtual_Networks.size());
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    path_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    vertex_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());

	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		path_Embeddings[vn_id][i].resize(m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.size());
		    
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    path_Embeddings[vn_id][i][neighbor_index].resize(depth, -1);
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 i = 0; i < m_last_vertex_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 vnet_id, virt_v_id, phys_v_id;		
		sInt_32 variable_ID = sABS(literal);

		decode_VertexEmbeddingMapping(variable_ID, vnet_id, virt_v_id, phys_v_id);

		vertex_Embeddings[vnet_id][virt_v_id] = phys_v_id;
	    }	    
	}
	for (sInt_32 i = m_last_vertex_mapping_variable - 1; i < m_last_edge_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 variable_ID = sABS(literal);
	    
		sInt_32 vnet_id;
		sInt_32 u_id, v_id, neighbor_index;
		sInt_32 phys_u_id = -1, path_index = -1;
		
		decode_LimitedEdgeEmbeddingMapping(variable_ID, vnet_id, u_id, v_id, neighbor_index, phys_u_id, path_index, depth);
		sASSERT(phys_u_id != -1 && path_index != -1);

//		sASSERT(path_Embeddings[vnet_id][u_id][neighbor_index][path_index] == -1);
		path_Embeddings[vnet_id][u_id][neighbor_index][path_index] = phys_u_id;
//		printf("vnet_id: %d, u_id: %d, v_id:%d, phys_u_id:%d, path_index:%d\n", vnet_id, u_id, v_id, phys_u_id, path_index);
	    }
	}
    }


    void sPathEmbeddingModel::decode_LazyTreeModel(Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings)
    {
	vertex_Embeddings.resize(m_virtual_Networks.size());
	path_Embeddings.resize(m_virtual_Networks.size());
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    path_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    vertex_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());

	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		path_Embeddings[vn_id][i].resize(m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.size());
		    
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    path_Embeddings[vn_id][i][neighbor_index].resize(m_physical_Network.get_VertexCount(), -1);
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 i = 0; i < m_last_vertex_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 vnet_id, virt_v_id, phys_v_id;		
		sInt_32 variable_ID = sABS(literal);

		decode_VertexEmbeddingMapping(variable_ID, vnet_id, virt_v_id, phys_v_id);

		vertex_Embeddings[vnet_id][virt_v_id] = phys_v_id;
	    }	    
	}
	for (sInt_32 i = m_last_vertex_mapping_variable - 1; i < m_last_edge_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 variable_ID = sABS(literal);
	    
		sInt_32 vnet_id;
		sInt_32 u_id, v_id, neighbor_index;
		sInt_32 phys_u_id = -1, path_index = -1;
		
		decode_EdgeEmbeddingMapping(variable_ID, vnet_id, u_id, v_id, neighbor_index, phys_u_id, path_index);
		sASSERT(phys_u_id != -1 && path_index != -1);

		path_Embeddings[vnet_id][u_id][neighbor_index][path_index] = phys_u_id;
//		printf("vnet_id: %d, u_id: %d, v_id:%d, phys_u_id:%d, path_index:%d\n", vnet_id, u_id, v_id, phys_u_id, path_index);
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    bool target_reached = false;
		    for (sInt_32 path_index = 0; path_index < m_physical_Network.get_VertexCount(); ++path_index)
		    {
			if (target_reached)
			{			    
			    path_Embeddings[vn_id][i][neighbor_index][path_index] = -1;
			}
			if (path_Embeddings[vn_id][i][neighbor_index][path_index] == vertex_Embeddings[vn_id][(*virt_neighbor)->m_target->m_id])
			{
			    target_reached = true;
			}
		    }
		    ++neighbor_index;
		}
	    }
	}       	
    }


    void sPathEmbeddingModel::decode_LazyFlatModel(Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkFlatMappings_vector &flat_Embeddings, EdgeNetworkFlatMappings_vector &flat_edge_Embeddings)    
    {
	vertex_Embeddings.resize(m_virtual_Networks.size());
	flat_Embeddings.resize(m_virtual_Networks.size());
	flat_edge_Embeddings.resize(m_virtual_Networks.size());	
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    flat_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    flat_edge_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());	    
	    vertex_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());

	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		flat_Embeddings[vn_id][i].resize(m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.size());
		flat_edge_Embeddings[vn_id][i].resize(m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.size());		

		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    flat_edge_Embeddings[vn_id][i][neighbor_index].resize(m_physical_Network.get_VertexCount());
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 i = 0; i < m_last_vertex_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 vnet_id, virt_v_id, phys_v_id;		
		sInt_32 variable_ID = sABS(literal);

		decode_VertexEmbeddingMapping(variable_ID, vnet_id, virt_v_id, phys_v_id);

		vertex_Embeddings[vnet_id][virt_v_id] = phys_v_id;
	    }	    
	}
	for (sInt_32 i = m_last_vertex_mapping_variable - 1; i < m_last_edge_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 variable_ID = sABS(literal);
	    
		sInt_32 vnet_id;
		sInt_32 u_id, v_id, neighbor_index;
		sInt_32 phys_u_id = -1;
		
		decode_FlatEdgeEmbeddingMapping(variable_ID, vnet_id, u_id, v_id, neighbor_index, phys_u_id);
		sASSERT(phys_u_id != -1);

		flat_Embeddings[vnet_id][u_id][neighbor_index].insert(phys_u_id);
		//printf("vnet_id: %d, u_id: %d, v_id:%d, phys_u_id:%d\n", vnet_id, u_id, v_id, phys_u_id);
	    }
	}
	for (sInt_32 i = m_last_edge_mapping_variable - 1; i < m_last_edge_neighbor_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 variable_ID = sABS(literal);
	    
		sInt_32 vnet_id;
		sInt_32 u_id, v_id, neighbor_index;
		sInt_32 phys_u_id = -1, phys_neighbor_index = -1;

		decode_FlatEdgeNeighborEmbeddingMapping(variable_ID, vnet_id, u_id, v_id, neighbor_index, phys_u_id, phys_neighbor_index);
		sASSERT(phys_u_id != -1 && phys_neighbor_index != -1);

		flat_edge_Embeddings[vnet_id][u_id][neighbor_index][phys_u_id].insert(phys_neighbor_index);
	    }	    
	}
    }    


    void sPathEmbeddingModel::decode_LimitedLazyTreeModel(Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkPathMappings_vector &path_Embeddings, sInt_32 depth)
    {
	vertex_Embeddings.resize(m_virtual_Networks.size());
	path_Embeddings.resize(m_virtual_Networks.size());
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    path_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    vertex_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());

	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		path_Embeddings[vn_id][i].resize(m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.size());
		    
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    path_Embeddings[vn_id][i][neighbor_index].resize(depth, -1);
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 i = 0; i < m_last_vertex_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 vnet_id, virt_v_id, phys_v_id;		
		sInt_32 variable_ID = sABS(literal);

		decode_VertexEmbeddingMapping(variable_ID, vnet_id, virt_v_id, phys_v_id);

		vertex_Embeddings[vnet_id][virt_v_id] = phys_v_id;
	    }	    
	}
	for (sInt_32 i = m_last_vertex_mapping_variable - 1; i < m_last_edge_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 variable_ID = sABS(literal);
	    
		sInt_32 vnet_id;
		sInt_32 u_id, v_id, neighbor_index;
		sInt_32 phys_u_id = -1, path_index = -1;
		
		decode_LimitedEdgeEmbeddingMapping(variable_ID, vnet_id, u_id, v_id, neighbor_index, phys_u_id, path_index, depth);
		sASSERT(phys_u_id != -1 && path_index != -1);

		sASSERT(path_Embeddings[vnet_id][u_id][neighbor_index][path_index] == -1);
		path_Embeddings[vnet_id][u_id][neighbor_index][path_index] = phys_u_id;
//		printf("vnet_id: %d, u_id: %d, v_id:%d, phys_u_id:%d, path_index:%d\n", vnet_id, u_id, v_id, phys_u_id, path_index);
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    bool target_reached = false;
		    for (sInt_32 path_index = 0; path_index < depth; ++path_index)
		    {
			if (target_reached)
			{			    
			    path_Embeddings[vn_id][i][neighbor_index][path_index] = -1;
			}
			if (path_Embeddings[vn_id][i][neighbor_index][path_index] == vertex_Embeddings[vn_id][(*virt_neighbor)->m_target->m_id])
			{
			    target_reached = true;
			}
		    }
		    ++neighbor_index;
		}
	    }
	}       
    }        


    void sPathEmbeddingModel::decode_LazyGraphModel(Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkGraphMappings_vector &graph_Embeddings)
    {
	vertex_Embeddings.resize(m_virtual_Networks.size());
	graph_Embeddings.resize(m_virtual_Networks.size());
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    graph_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    vertex_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());

	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		graph_Embeddings[vn_id][i].resize(m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.size());
		    
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    graph_Embeddings[vn_id][i][neighbor_index].resize(m_physical_Network.get_VertexCount());
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 i = 0; i < m_last_vertex_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 vnet_id, virt_v_id, phys_v_id;		
		sInt_32 variable_ID = sABS(literal);

		decode_VertexEmbeddingMapping(variable_ID, vnet_id, virt_v_id, phys_v_id);

		vertex_Embeddings[vnet_id][virt_v_id] = phys_v_id;
	    }	    
	}
	for (sInt_32 i = m_last_vertex_mapping_variable - 1; i < m_last_edge_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 variable_ID = sABS(literal);
	    
		sInt_32 vnet_id;
		sInt_32 u_id, v_id, neighbor_index;
		sInt_32 phys_u_id = -1, path_index = -1;
		
		decode_EdgeEmbeddingMapping(variable_ID, vnet_id, u_id, v_id, neighbor_index, phys_u_id, path_index);
		sASSERT(phys_u_id != -1 && path_index != -1);

		graph_Embeddings[vnet_id][u_id][neighbor_index][path_index].push_back(phys_u_id);
//		printf("vnet_id: %d, u_id: %d, v_id:%d, phys_u_id:%d, path_index:%d\n", vnet_id, u_id, v_id, phys_u_id, path_index);
	    }
	}
    }


    void sPathEmbeddingModel::decode_LimitedLazyGraphModel(Glucose::Solver *solver, Mappings_vector &vertex_Embeddings, NetworkGraphMappings_vector &graph_Embeddings, sInt_32 depth)
    {
	vertex_Embeddings.resize(m_virtual_Networks.size());
	graph_Embeddings.resize(m_virtual_Networks.size());
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    graph_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());
	    vertex_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());

	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		graph_Embeddings[vn_id][i].resize(m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.size());
		    
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    graph_Embeddings[vn_id][i][neighbor_index].resize(depth);
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 i = 0; i < m_last_vertex_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 vnet_id, virt_v_id, phys_v_id;		
		sInt_32 variable_ID = sABS(literal);

		decode_VertexEmbeddingMapping(variable_ID, vnet_id, virt_v_id, phys_v_id);

		vertex_Embeddings[vnet_id][virt_v_id] = phys_v_id;
	    }	    
	}
	for (sInt_32 i = m_last_vertex_mapping_variable - 1; i < m_last_edge_mapping_variable - 1; i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 variable_ID = sABS(literal);
	    
		sInt_32 vnet_id;
		sInt_32 u_id, v_id, neighbor_index;
		sInt_32 phys_u_id = -1, path_index = -1;
		
		decode_LimitedEdgeEmbeddingMapping(variable_ID, vnet_id, u_id, v_id, neighbor_index, phys_u_id, path_index, depth);
		sASSERT(phys_u_id != -1 && path_index != -1);

		//sASSERT(path_Embeddings[vnet_id][u_id][neighbor_index][path_index] == -1);
		graph_Embeddings[vnet_id][u_id][neighbor_index][path_index].push_back(phys_u_id);
		//printf("vnet_id: %d, u_id: %d, v_id:%d, phys_u_id:%d, path_index:%d\n", vnet_id, u_id, v_id, phys_u_id, path_index);
	    }
	}
    }


    void sPathEmbeddingModel::transform_LimitedGraph2PathEmbeddings(const Mappings_vector &vertex_Embeddings, const NetworkGraphMappings_vector &graph_Embeddings, NetworkPathMappings_vector &path_Embeddings, sInt_32 depth)
    {
	path_Embeddings.resize(m_virtual_Networks.size());
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    path_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());

	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		path_Embeddings[vn_id][i].resize(m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.size());
		    
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    path_Embeddings[vn_id][i][neighbor_index].resize(depth, -1);
		    ++neighbor_index;
		}
	    }
	}
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    std::vector<std::vector<sInt_32> > previous;
		    previous.resize(depth);
		    previous[1].resize(graph_Embeddings[vn_id][i][neighbor_index][1].size(), -1);

		    for (sInt_32 ed1 = 0; ed1 < graph_Embeddings[vn_id][i][neighbor_index][0].size(); ++ed1)
		    {
			if (graph_Embeddings[vn_id][i][neighbor_index][0][ed1] == vertex_Embeddings[vn_id][i])
			{
			    for (sInt_32 ed2 = 0; ed2 < graph_Embeddings[vn_id][i][neighbor_index][1].size(); ++ed2)
			    {
				if (previous[1][ed2] == -1)
				{
				    if (m_physical_Network.is_LinkedTo(graph_Embeddings[vn_id][i][neighbor_index][0][ed1], graph_Embeddings[vn_id][i][neighbor_index][1][ed2]))
				    {
					previous[1][ed2] = ed1;
				    }
				}
			    }
			}
		    }
		    
		    for (sInt_32 d = 1; d < depth - 1; ++d)
		    {
			previous[d+1].resize(graph_Embeddings[vn_id][i][neighbor_index][d+1].size(), -1);
			for (sInt_32 ed1 = 0; ed1 < graph_Embeddings[vn_id][i][neighbor_index][d].size(); ++ed1)
			{
			    for (sInt_32 ed2 = 0; ed2 < graph_Embeddings[vn_id][i][neighbor_index][d+1].size(); ++ed2)
			    {
				if (previous[d+1][ed2] == -1 && previous[d][ed1] != -1)
				{				    
				    if (m_physical_Network.is_LinkedTo(graph_Embeddings[vn_id][i][neighbor_index][d][ed1], graph_Embeddings[vn_id][i][neighbor_index][d+1][ed2]))
				    {
					previous[d+1][ed2] = ed1;
				    }
				}				
			    } 
			}
		    }

/*
		    for (sInt_32 ii = 0; ii < depth; ++ii)
		    {
			printf("%d: ", ii);
			for (sInt_32 jj = 0; jj < previous[ii].size(); ++jj)
			{
			    if (previous[ii][jj] >= 0)
			    {		    
				printf("%d (%d)\t", previous[ii][jj], graph_Embeddings[vn_id][i][neighbor_index][ii-1][previous[ii][jj]]);
			    }
			    else
			    {
				printf("-1\t");
			    }
			}
			printf("\n");
		    }
*/
		    for (sInt_32 d = 1; d < depth; ++d)
		    {
			for (sInt_32 ed = 0; ed < graph_Embeddings[vn_id][i][neighbor_index][d].size(); ++ed)
			{
			    if (vertex_Embeddings[vn_id][(*virt_neighbor)->m_target->m_id] == graph_Embeddings[vn_id][i][neighbor_index][d][ed])
			    {				
				if (previous[d][ed] != -1)
				{
				    sInt_32 prev = ed;
				    path_Embeddings[vn_id][i][neighbor_index][d] = graph_Embeddings[vn_id][i][neighbor_index][d][ed];
				    sInt_32 dd = d;

				    while (dd > 0)
				    {
					prev = previous[dd][prev];
					--dd;
					path_Embeddings[vn_id][i][neighbor_index][dd] = graph_Embeddings[vn_id][i][neighbor_index][dd][prev];
				    }
				}
			    }
			}
		    }

/*		    
		    printf("Fino path for: %d -- %d\n", vertex_Embeddings[vn_id][i], vertex_Embeddings[vn_id][(*virt_neighbor)->m_target->m_id]);
		    for (sInt_32 dd = 0; dd < depth; ++dd)
		    {
			if (path_Embeddings[vn_id][i][neighbor_index][dd] != -1)
			{
			    printf("%d ", path_Embeddings[vn_id][i][neighbor_index][dd]);
			}
			else
			{
			    break;
			}
		    }
		    printf("\n");
*/
		    ++neighbor_index;		    
		}
	    }
	}
    }


    void sPathEmbeddingModel::transform_Flat2PathEmbeddings(const Mappings_vector &vertex_Embeddings, const NetworkFlatMappings_vector &flat_Embeddings, const EdgeNetworkFlatMappings_vector &flat_edge_Embeddings, NetworkPathMappings_vector &path_Embeddings)
    {
	path_Embeddings.resize(m_virtual_Networks.size());
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    path_Embeddings[vn_id].resize(m_virtual_Networks[vn_id].get_VertexCount());

	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		path_Embeddings[vn_id][i].resize(m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.size());
	    }
	}
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 i = 0; i < m_virtual_Networks[vn_id].get_VertexCount(); ++i)
	    {
		sInt_32 neighbor_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[i].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 phys_u_id = vertex_Embeddings[vn_id][i];
		    sInt_32 phys_v_id = vertex_Embeddings[vn_id][(*virt_neighbor)->m_target->m_id];

		    expand_FlatPath(phys_u_id, phys_v_id, flat_Embeddings[vn_id][i][neighbor_index], flat_edge_Embeddings[vn_id][i][neighbor_index], path_Embeddings[vn_id][i][neighbor_index]);
		    ++neighbor_index;		    
		}
	    }
	}
    }


    void sPathEmbeddingModel::expand_FlatPath(sInt_32 phys_u_id, sInt_32 phys_v_id, const FlatMapping_set &flat_Path, const EdgePhysicalVertexMapping_vector &flat_edge_Path, PathMapping_vector &path)       
    {
	sASSERT(!flat_Path.empty());
	
	for (sInt_32 phys_vertex_id = 0; phys_vertex_id < m_physical_Network.get_VertexCount(); ++phys_vertex_id)
	{
	    m_physical_Network.m_Vertices[phys_vertex_id].m_visited = false;
	    m_physical_Network.m_Vertices[phys_vertex_id].m_prev_id = -1;	    
	}
	
	std::list<sInt_32> queue;	
	queue.push_back(phys_u_id);
	m_physical_Network.m_Vertices[phys_u_id].m_visited = true;

	/*
	printf("Flat path:\n");
	for (FlatMapping_set::const_iterator flat_vertex = flat_Path.begin(); flat_vertex != flat_Path.end(); ++flat_vertex)
	{
	    printf("%d ", *flat_vertex);
	}
	printf("\n");

	printf("Flat edge path:\n");
	for (sInt_32 k = 0; k < flat_edge_Path.size(); ++k)
	{
	    if (!flat_edge_Path[k].empty())
	    {
		printf("%d (", k);
		for (FlatMapping_set::const_iterator flat_neighbor = flat_edge_Path[k].begin(); flat_neighbor != flat_edge_Path[k].end(); ++flat_neighbor)
		{
		    printf("%d ", m_physical_Network.m_Vertices[k].m_out_Neighbors[*flat_neighbor]->m_target->m_id);
		}
		printf(") ");
	    }
	}
	printf("\n");
	*/

	while (!queue.empty())
	{
	    sInt_32 phys_vertex_id = queue.front();
	    queue.pop_front();

	    if (phys_vertex_id == phys_v_id)
	    {
		std::vector<sInt_32> anti_path;		
		sInt_32 vertex_id = phys_vertex_id;
		
		while (vertex_id >= 0)
		{
		    anti_path.push_back(vertex_id);
		    vertex_id = m_physical_Network.m_Vertices[vertex_id].m_prev_id;
		}
		for (sInt_32 i = anti_path.size() - 1; i >= 0; --i)
		{
		    path.push_back(anti_path[i]);
		}

		/*
		printf("Regular path:\n");
		for (PathMapping_vector::const_iterator vertex = path.begin(); vertex != path.end(); ++vertex)
		{
		    printf("%d ", *vertex);
		}
		printf("\n");
		//getchar();
		*/
		return;
	    }
	    sInt_32 phys_neighbor_index = 0;
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		sInt_32 target_id = (*phys_neighbor)->m_target->m_id;

		if (flat_Path.find(target_id) != flat_Path.end() && flat_edge_Path[phys_vertex_id].find(phys_neighbor_index) != flat_edge_Path[phys_vertex_id].end())
		{
		    if (!m_physical_Network.m_Vertices[target_id].m_visited)
		    {
			m_physical_Network.m_Vertices[target_id].m_visited = true;
			m_physical_Network.m_Vertices[target_id].m_prev_id = phys_vertex_id;
		    
			queue.push_back(target_id);
		    }
		}
		++phys_neighbor_index;
	    }
	}
    }	   
    

    bool sPathEmbeddingModel::refine_LazyPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &virtual_Embeddings, const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings)
    {
	bool refined = false;
	
	vector<sDouble> vertex_Saturations;
	vector<sBoolEncoder::VariableIDs_vector> vertex_Nogoods;

	typedef vector<sBoolEncoder::VariableIDs_vector> EdgeNogood_vector;
	typedef vector<EdgeNogood_vector> EdgeNogoods_vector;
	EdgeNogoods_vector edge_Nogoods;

	typedef vector<sDouble> Saturations_vector;
	typedef vector<Saturations_vector> EdgeSaturations_vector;
	EdgeSaturations_vector edge_Saturations;

	edge_Saturations.resize(m_physical_Network.get_VertexCount());
	edge_Nogoods.resize(m_physical_Network.get_VertexCount());	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    edge_Saturations[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size(), 0.0);
	    edge_Nogoods[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size());	    
	}
	
	vertex_Saturations.resize(m_physical_Network.get_VertexCount(), 0.0);
	vertex_Nogoods.resize(m_physical_Network.get_VertexCount());

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
//		printf("%d <=+ %.3f\n", virtual_Embeddings[vn_id][virt_v_id], m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity);
		
		vertex_Saturations[virtual_Embeddings[vn_id][virt_v_id]] += m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity;
//		printf("  %d,%d,%d\n", vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]);
		vertex_Nogoods[virtual_Embeddings[vn_id][virt_v_id]].push_back(calc_VertexEmbeddingBitVariableID(vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]));
	    }	    
	}	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
//	    printf("Saturs %d: %.3f\n", phys_v_id, vertex_Saturations[phys_v_id]);
	    if (vertex_Saturations[phys_v_id] > m_physical_Network.m_Vertices[phys_v_id].m_capacity + s_EPSILON)
	    {
		refined = true;
/*
		printf("refining...\n");
		for (sInt_32 var_id = 0; var_id < vertex_Nogoods[phys_v_id].size(); ++var_id)
		{
		    printf("    %d\n", vertex_Nogoods[phys_v_id][var_id]);
		}
*/
		encoder->cast_BigMutex(solver, vertex_Nogoods[phys_v_id]);
//		encoder->cast_Mutex(solver, vertex_Nogoods[phys_v_id][0], vertex_Nogoods[phys_v_id][1]);
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 last_vertex_id = -1;
		    sInt_32 last_nogood_var = -1;
		    for (sInt_32 path_index = 0; path_index < m_physical_Network.get_VertexCount(); ++path_index)
//		    for (sInt_32 path_index = 0; path_index < m_virtual_Networks[vn_id].get_VertexCount(); ++path_index)
		    {
			sDouble capacity = (*virt_neighbor)->m_capacity;
			sInt_32 path_vertex_id = path_Embeddings[vn_id][virt_v_id][neigh_index][path_index];

			sInt_32 nogood_variable = calc_EdgeEmbeddingBitVariableID(vn_id, virt_v_id, neigh_index, path_index, path_vertex_id);

			if (last_vertex_id >= 0 && path_vertex_id >= 0)
			{			    
			    sInt_32 phys_neighbor_index = -1, phys_neigh_index = 0;
			    
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
			    {
				if ((*phys_neighbor)->m_target->m_id == path_vertex_id)
				{
				    phys_neighbor_index = phys_neigh_index;
				    break;
				}
				++phys_neigh_index;
			    }
			    sASSERT(phys_neighbor_index != -1);

			    edge_Saturations[last_vertex_id][phys_neighbor_index] += capacity;
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(last_nogood_var);
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(nogood_variable);
			}
			last_vertex_id = path_vertex_id;
			last_nogood_var = nogood_variable;
		    }
		    ++neigh_index;
		}
	    }
	}

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sInt_32 phys_neigh_index = 0;
	    //printf("phys v: %d:  ", phys_v_id);
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		//printf("%.3f (%.3f) ", edge_Saturations[phys_v_id][phys_neigh_index], (*phys_neighbor)->m_capacity);
		if (edge_Saturations[phys_v_id][phys_neigh_index] > (*phys_neighbor)->m_capacity + s_EPSILON)
		{
		    refined = true;		    
		    encoder->cast_BigMutex(solver, edge_Nogoods[phys_v_id][phys_neigh_index]);
		    //printf("\nNOGOOD GENE:%d\n", edge_Nogoods[phys_v_id][phys_neigh_index].size());
		}
		++phys_neigh_index;
	    }
	    printf("\n");
	}
	
	return refined;
    }


    bool sPathEmbeddingModel::refine_LimitedLazyPathModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &virtual_Embeddings, const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings, sInt_32 depth)
    {
	bool refined = false;
	
	vector<sDouble> vertex_Saturations;
	vector<sBoolEncoder::VariableIDs_vector> vertex_Nogoods;

	typedef vector<sBoolEncoder::VariableIDs_vector> EdgeNogood_vector;
	typedef vector<EdgeNogood_vector> EdgeNogoods_vector;
	EdgeNogoods_vector edge_Nogoods;

	typedef vector<sDouble> Saturations_vector;
	typedef vector<Saturations_vector> EdgeSaturations_vector;
	EdgeSaturations_vector edge_Saturations;

	edge_Saturations.resize(m_physical_Network.get_VertexCount());
	edge_Nogoods.resize(m_physical_Network.get_VertexCount());	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    edge_Saturations[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size(), 0.0);
	    edge_Nogoods[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size());	    
	}
	
	vertex_Saturations.resize(m_physical_Network.get_VertexCount(), 0.0);
	vertex_Nogoods.resize(m_physical_Network.get_VertexCount());

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
//		printf("%d <=+ %.3f\n", virtual_Embeddings[vn_id][virt_v_id], m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity);
		
		vertex_Saturations[virtual_Embeddings[vn_id][virt_v_id]] += m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity;
//		printf("  %d,%d,%d\n", vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]);
		vertex_Nogoods[virtual_Embeddings[vn_id][virt_v_id]].push_back(calc_VertexEmbeddingBitVariableID(vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]));
	    }	    
	}	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
//	    printf("Saturs %d: %.3f\n", phys_v_id, vertex_Saturations[phys_v_id]);
	    if (vertex_Saturations[phys_v_id] > m_physical_Network.m_Vertices[phys_v_id].m_capacity + s_EPSILON)
	    {
		refined = true;
/*
		printf("refining...\n");
		for (sInt_32 var_id = 0; var_id < vertex_Nogoods[phys_v_id].size(); ++var_id)
		{
		    printf("    %d\n", vertex_Nogoods[phys_v_id][var_id]);
		}
*/
		encoder->cast_BigMutex(solver, vertex_Nogoods[phys_v_id]);
//		encoder->cast_Mutex(solver, vertex_Nogoods[phys_v_id][0], vertex_Nogoods[phys_v_id][1]);
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 last_vertex_id = -1;
		    sInt_32 last_nogood_var = -1;
//		    for (sInt_32 path_index = 0; path_index < depth; ++path_index)
		    for (sInt_32 path_index = 0; path_index < path_Embeddings[vn_id][virt_v_id][neigh_index].size(); ++path_index)
		    {
			sDouble capacity = (*virt_neighbor)->m_capacity;
			sInt_32 path_vertex_id = path_Embeddings[vn_id][virt_v_id][neigh_index][path_index];
			
			printf("%d ", path_vertex_id);
			
			if (path_vertex_id < 0)
			{
			    break;
			}

			sInt_32 nogood_variable = calc_EdgeEmbeddingBitVariableID(vn_id, virt_v_id, neigh_index, path_index, path_vertex_id);

			if (last_vertex_id >= 0 && path_vertex_id >= 0)
			{
			    sInt_32 phys_neighbor_index = -1, phys_neigh_index = 0;
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
			    {
				if ((*phys_neighbor)->m_target->m_id == path_vertex_id)
				{
				    phys_neighbor_index = phys_neigh_index;
				    break;
				}
				++phys_neigh_index;
			    }			    
			    sASSERT(phys_neighbor_index != -1);

			    edge_Saturations[last_vertex_id][phys_neighbor_index] += capacity;
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(last_nogood_var);
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(nogood_variable);
			}
			last_vertex_id = path_vertex_id;
			last_nogood_var = nogood_variable;
		    }
		    printf("\n");
		    ++neigh_index;
		}
	    }
	}

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sInt_32 phys_neigh_index = 0;
	    //printf("phys v: %d:  ", phys_v_id);
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		//printf("%.3f (%.3f) ", edge_Saturations[phys_v_id][phys_neigh_index], (*phys_neighbor)->m_capacity);
		if (edge_Saturations[phys_v_id][phys_neigh_index] > (*phys_neighbor)->m_capacity + s_EPSILON)
		{
		    refined = true;		    
		    encoder->cast_BigMutex(solver, edge_Nogoods[phys_v_id][phys_neigh_index]);
		    //printf("\nNOGOOD GENE:%d\n", edge_Nogoods[phys_v_id][phys_neigh_index].size());
		}
		++phys_neigh_index;
	    }
	    //printf("\n");
	}
	
	return refined;
    }


    bool sPathEmbeddingModel::refine_LazyTreeModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &virtual_Embeddings, const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings)
    {
	bool refined = false;
	
	vector<sDouble> vertex_Saturations;
	vector<sBoolEncoder::VariableIDs_vector> vertex_Nogoods;

	typedef vector<sBoolEncoder::VariableIDs_vector> EdgeNogood_vector;
	typedef vector<EdgeNogood_vector> EdgeNogoods_vector;
	EdgeNogoods_vector edge_Nogoods;

	typedef vector<sDouble> Saturations_vector;
	typedef vector<Saturations_vector> EdgeSaturations_vector;
	EdgeSaturations_vector edge_Saturations;

	edge_Saturations.resize(m_physical_Network.get_VertexCount());
	edge_Nogoods.resize(m_physical_Network.get_VertexCount());	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    edge_Saturations[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size(), 0.0);
	    edge_Nogoods[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size());	    
	}
	
	vertex_Saturations.resize(m_physical_Network.get_VertexCount(), 0.0);
	vertex_Nogoods.resize(m_physical_Network.get_VertexCount());

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
//		printf("%d <=+ %.3f\n", virtual_Embeddings[vn_id][virt_v_id], m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity);
		
		vertex_Saturations[virtual_Embeddings[vn_id][virt_v_id]] += m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity;
//		printf("  %d,%d,%d\n", vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]);
		vertex_Nogoods[virtual_Embeddings[vn_id][virt_v_id]].push_back(calc_VertexEmbeddingBitVariableID(vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]));
	    }	    
	}	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
//	    printf("Saturs %d: %.3f\n", phys_v_id, vertex_Saturations[phys_v_id]);
	    if (vertex_Saturations[phys_v_id] > m_physical_Network.m_Vertices[phys_v_id].m_capacity + s_EPSILON)
	    {
		refined = true;
/*
		printf("refining...\n");
		for (sInt_32 var_id = 0; var_id < vertex_Nogoods[phys_v_id].size(); ++var_id)
		{
		    printf("    %d\n", vertex_Nogoods[phys_v_id][var_id]);
		}
*/
		encoder->cast_BigMutex(solver, vertex_Nogoods[phys_v_id]);
//		encoder->cast_Mutex(solver, vertex_Nogoods[phys_v_id][0], vertex_Nogoods[phys_v_id][1]);
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 last_vertex_id = -1;
		    sInt_32 last_nogood_var = -1;
		    for (sInt_32 path_index = 0; path_index < m_physical_Network.get_VertexCount(); ++path_index)
//		    for (sInt_32 path_index = 0; path_index < m_virtual_Networks[vn_id].get_VertexCount(); ++path_index)
		    {
			sDouble capacity = (*virt_neighbor)->m_capacity;
			sInt_32 path_vertex_id = path_Embeddings[vn_id][virt_v_id][neigh_index][path_index];

			sInt_32 nogood_variable = calc_EdgeEmbeddingBitVariableID(vn_id, virt_v_id, neigh_index, path_index, path_vertex_id);

			if (last_vertex_id >= 0 && path_vertex_id >= 0)
			{			    
			    sInt_32 phys_neighbor_index = -1, phys_neigh_index = 0;
			    
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
			    {
				if ((*phys_neighbor)->m_target->m_id == path_vertex_id)
				{
				    phys_neighbor_index = phys_neigh_index;
				    break;
				}
				++phys_neigh_index;
			    }
			    sASSERT(phys_neighbor_index != -1);

			    edge_Saturations[last_vertex_id][phys_neighbor_index] += capacity;
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(last_nogood_var);
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(nogood_variable);
			}
			last_vertex_id = path_vertex_id;
			last_nogood_var = nogood_variable;
		    }
		    ++neigh_index;
		}
	    }
	}

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sInt_32 phys_neigh_index = 0;
	    //printf("phys v: %d:  ", phys_v_id);
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		//printf("%.3f (%.3f) ", edge_Saturations[phys_v_id][phys_neigh_index], (*phys_neighbor)->m_capacity);
		if (edge_Saturations[phys_v_id][phys_neigh_index] > (*phys_neighbor)->m_capacity + s_EPSILON)
		{
		    refined = true;		    
		    encoder->cast_BigMutex(solver, edge_Nogoods[phys_v_id][phys_neigh_index]);
		    //printf("\nNOGOOD GENE:%d\n", edge_Nogoods[phys_v_id][phys_neigh_index].size());
		}
		++phys_neigh_index;
	    }
	    //printf("\n");
	}
	
	return refined;
    }


    bool sPathEmbeddingModel::refine_LimitedLazyTreeModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &virtual_Embeddings, const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings, sInt_32 depth)
    {
	bool refined = false;
	
	vector<sDouble> vertex_Saturations;
	vector<sBoolEncoder::VariableIDs_vector> vertex_Nogoods;

	typedef vector<sBoolEncoder::VariableIDs_vector> EdgeNogood_vector;
	typedef vector<EdgeNogood_vector> EdgeNogoods_vector;
	EdgeNogoods_vector edge_Nogoods;

	typedef vector<sDouble> Saturations_vector;
	typedef vector<Saturations_vector> EdgeSaturations_vector;
	EdgeSaturations_vector edge_Saturations;

	edge_Saturations.resize(m_physical_Network.get_VertexCount());
	edge_Nogoods.resize(m_physical_Network.get_VertexCount());	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    edge_Saturations[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size(), 0.0);
	    edge_Nogoods[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size());	    
	}
	
	vertex_Saturations.resize(m_physical_Network.get_VertexCount(), 0.0);
	vertex_Nogoods.resize(m_physical_Network.get_VertexCount());

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
//		printf("%d <=+ %.3f\n", virtual_Embeddings[vn_id][virt_v_id], m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity);
		
		vertex_Saturations[virtual_Embeddings[vn_id][virt_v_id]] += m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity;
//		printf("  %d,%d,%d\n", vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]);
		vertex_Nogoods[virtual_Embeddings[vn_id][virt_v_id]].push_back(calc_VertexEmbeddingBitVariableID(vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]));
	    }	    
	}	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
//	    printf("Saturs %d: %.3f\n", phys_v_id, vertex_Saturations[phys_v_id]);
	    if (vertex_Saturations[phys_v_id] > m_physical_Network.m_Vertices[phys_v_id].m_capacity + s_EPSILON)
	    {
		refined = true;
/*
		printf("refining...\n");
		for (sInt_32 var_id = 0; var_id < vertex_Nogoods[phys_v_id].size(); ++var_id)
		{
		    printf("    %d\n", vertex_Nogoods[phys_v_id][var_id]);
		}
*/
		encoder->cast_BigMutex(solver, vertex_Nogoods[phys_v_id]);
//		encoder->cast_Mutex(solver, vertex_Nogoods[phys_v_id][0], vertex_Nogoods[phys_v_id][1]);
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    //m_virtual_Networks[vn_id].to_Screen();
	    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 last_vertex_id = -1;
		    sInt_32 last_nogood_var = -1;
		    for (sInt_32 path_index = 0; path_index < depth; ++path_index)
//		    for (sInt_32 path_index = 0; path_index < m_virtual_Networks[vn_id].get_VertexCount(); ++path_index)
		    {			
			sDouble capacity = (*virt_neighbor)->m_capacity;
			sInt_32 path_vertex_id = path_Embeddings[vn_id][virt_v_id][neigh_index][path_index];

			sInt_32 nogood_variable = calc_EdgeEmbeddingBitVariableID(vn_id, virt_v_id, neigh_index, path_index, path_vertex_id);

			if (last_vertex_id >= 0 && path_vertex_id >= 0)
			{
			    sInt_32 phys_neighbor_index = -1, phys_neigh_index = 0;
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
			    {
				if ((*phys_neighbor)->m_target->m_id == path_vertex_id)
				{
				    phys_neighbor_index = phys_neigh_index;
				    break;
				}
				++phys_neigh_index;
			    }			    
			    sASSERT(phys_neighbor_index != -1);

			    edge_Saturations[last_vertex_id][phys_neighbor_index] += capacity;
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(last_nogood_var);
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(nogood_variable);
			}
			last_vertex_id = path_vertex_id;
			last_nogood_var = nogood_variable;
		    }
		    ++neigh_index;
		}
	    }
	}

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sInt_32 phys_neigh_index = 0;
	    //printf("phys v: %d:  ", phys_v_id);
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		//printf("%.3f (%.3f) ", edge_Saturations[phys_v_id][phys_neigh_index], (*phys_neighbor)->m_capacity);
		if (edge_Saturations[phys_v_id][phys_neigh_index] > (*phys_neighbor)->m_capacity + s_EPSILON)
		{
		    refined = true;		    
		    encoder->cast_BigMutex(solver, edge_Nogoods[phys_v_id][phys_neigh_index]);
		    //printf("\nNOGOOD GENE:%d\n", edge_Nogoods[phys_v_id][phys_neigh_index].size());
		}
		++phys_neigh_index;
	    }
	    //printf("\n");
	}
	
	return refined;
    }        


    bool sPathEmbeddingModel::refine_LazyGraphModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &virtual_Embeddings, const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings)
    {
	bool refined = false;
	
	vector<sDouble> vertex_Saturations;
	vector<sBoolEncoder::VariableIDs_vector> vertex_Nogoods;

	typedef vector<sBoolEncoder::VariableIDs_vector> EdgeNogood_vector;
	typedef vector<EdgeNogood_vector> EdgeNogoods_vector;
	EdgeNogoods_vector edge_Nogoods;

	typedef vector<sDouble> Saturations_vector;
	typedef vector<Saturations_vector> EdgeSaturations_vector;
	EdgeSaturations_vector edge_Saturations;

	edge_Saturations.resize(m_physical_Network.get_VertexCount());
	edge_Nogoods.resize(m_physical_Network.get_VertexCount());	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    edge_Saturations[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size(), 0.0);
	    edge_Nogoods[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size());	    
	}
	
	vertex_Saturations.resize(m_physical_Network.get_VertexCount(), 0.0);
	vertex_Nogoods.resize(m_physical_Network.get_VertexCount());

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
//		printf("%d <=+ %.3f\n", virtual_Embeddings[vn_id][virt_v_id], m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity);
		
		vertex_Saturations[virtual_Embeddings[vn_id][virt_v_id]] += m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity;
//		printf("  %d,%d,%d\n", vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]);
		vertex_Nogoods[virtual_Embeddings[vn_id][virt_v_id]].push_back(calc_VertexEmbeddingBitVariableID(vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]));
	    }	    
	}	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
//	    printf("Saturs %d: %.3f\n", phys_v_id, vertex_Saturations[phys_v_id]);
	    if (vertex_Saturations[phys_v_id] > m_physical_Network.m_Vertices[phys_v_id].m_capacity + s_EPSILON)
	    {
		refined = true;
/*
		printf("refining...\n");
		for (sInt_32 var_id = 0; var_id < vertex_Nogoods[phys_v_id].size(); ++var_id)
		{
		    printf("    %d\n", vertex_Nogoods[phys_v_id][var_id]);
		}
*/
		encoder->cast_BigMutex(solver, vertex_Nogoods[phys_v_id]);
//		encoder->cast_Mutex(solver, vertex_Nogoods[phys_v_id][0], vertex_Nogoods[phys_v_id][1]);
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 last_vertex_id = -1;
		    sInt_32 last_nogood_var = -1;
		    for (sInt_32 path_index = 0; path_index < m_physical_Network.get_VertexCount(); ++path_index)
//		    for (sInt_32 path_index = 0; path_index < m_virtual_Networks[vn_id].get_VertexCount(); ++path_index)
		    {
			sDouble capacity = (*virt_neighbor)->m_capacity;
			sInt_32 path_vertex_id = path_Embeddings[vn_id][virt_v_id][neigh_index][path_index];

			sInt_32 nogood_variable = calc_EdgeEmbeddingBitVariableID(vn_id, virt_v_id, neigh_index, path_index, path_vertex_id);
			
			if (last_vertex_id >= 0 && path_vertex_id >= 0)
			{			    
			    sInt_32 phys_neighbor_index = -1, phys_neigh_index = 0;
				
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
			    {
				if ((*phys_neighbor)->m_target->m_id == path_vertex_id)
				{
				    phys_neighbor_index = phys_neigh_index;
				    break;
				}
				++phys_neigh_index;
			    }
			    sASSERT(phys_neighbor_index != -1);

			    edge_Saturations[last_vertex_id][phys_neighbor_index] += capacity;
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(last_nogood_var);
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(nogood_variable);
			}
			last_vertex_id = path_vertex_id;
			last_nogood_var = nogood_variable;			    
		    }
		    ++neigh_index;
		}
	    }
	}

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sInt_32 phys_neigh_index = 0;
	    //printf("phys v: %d:  ", phys_v_id);
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		//printf("%.3f (%.3f) ", edge_Saturations[phys_v_id][phys_neigh_index], (*phys_neighbor)->m_capacity);
		if (edge_Saturations[phys_v_id][phys_neigh_index] > (*phys_neighbor)->m_capacity + s_EPSILON)
		{
		    refined = true;		    
		    encoder->cast_BigMutex(solver, edge_Nogoods[phys_v_id][phys_neigh_index]);
		    //printf("\nNOGOOD GENE:%d\n", edge_Nogoods[phys_v_id][phys_neigh_index].size());
		}
		++phys_neigh_index;
	    }
	    //printf("\n");
	}
	
	return refined;
    }


    bool sPathEmbeddingModel::refine_LimitedLazyGraphModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &virtual_Embeddings, const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings, sInt_32 depth)
    {
	bool refined = false;
	
	vector<sDouble> vertex_Saturations;
	vector<sBoolEncoder::VariableIDs_vector> vertex_Nogoods;

	typedef vector<sBoolEncoder::VariableIDs_vector> EdgeNogood_vector;
	typedef vector<EdgeNogood_vector> EdgeNogoods_vector;
	EdgeNogoods_vector edge_Nogoods;

	typedef vector<sDouble> Saturations_vector;
	typedef vector<Saturations_vector> EdgeSaturations_vector;
	EdgeSaturations_vector edge_Saturations;

	edge_Saturations.resize(m_physical_Network.get_VertexCount());
	edge_Nogoods.resize(m_physical_Network.get_VertexCount());	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    edge_Saturations[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size(), 0.0);
	    edge_Nogoods[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size());	    
	}
	
	vertex_Saturations.resize(m_physical_Network.get_VertexCount(), 0.0);
	vertex_Nogoods.resize(m_physical_Network.get_VertexCount());

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
//		printf("%d <=+ %.3f\n", virtual_Embeddings[vn_id][virt_v_id], m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity);
		
		vertex_Saturations[virtual_Embeddings[vn_id][virt_v_id]] += m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity;
//		printf("  %d,%d,%d\n", vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]);
		vertex_Nogoods[virtual_Embeddings[vn_id][virt_v_id]].push_back(calc_VertexEmbeddingBitVariableID(vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]));
	    }	    
	}	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
//	    printf("Saturs %d: %.3f\n", phys_v_id, vertex_Saturations[phys_v_id]);
	    if (vertex_Saturations[phys_v_id] > m_physical_Network.m_Vertices[phys_v_id].m_capacity + s_EPSILON)
	    {
		refined = true;
/*
		printf("refining...\n");
		for (sInt_32 var_id = 0; var_id < vertex_Nogoods[phys_v_id].size(); ++var_id)
		{
		    printf("    %d\n", vertex_Nogoods[phys_v_id][var_id]);
		}
*/
		encoder->cast_BigMutex(solver, vertex_Nogoods[phys_v_id]);
//		encoder->cast_Mutex(solver, vertex_Nogoods[phys_v_id][0], vertex_Nogoods[phys_v_id][1]);
	    }
	}

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    //m_virtual_Networks[vn_id].to_Screen();
	    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 last_vertex_id = -1;
		    sInt_32 last_nogood_var = -1;
		    for (sInt_32 path_index = 0; path_index < depth; ++path_index)
//		    for (sInt_32 path_index = 0; path_index < m_virtual_Networks[vn_id].get_VertexCount(); ++path_index)
		    {			
			sDouble capacity = (*virt_neighbor)->m_capacity;
			sInt_32 path_vertex_id = path_Embeddings[vn_id][virt_v_id][neigh_index][path_index];

			sInt_32 nogood_variable = calc_EdgeEmbeddingBitVariableID(vn_id, virt_v_id, neigh_index, path_index, path_vertex_id);
			    
			if (last_vertex_id >= 0 && path_vertex_id >= 0)
			{
			    sInt_32 phys_neighbor_index = -1, phys_neigh_index = 0;
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
			    {
				if ((*phys_neighbor)->m_target->m_id == path_vertex_id)
				{
				    phys_neighbor_index = phys_neigh_index;
				    break;
				}
				++phys_neigh_index;
			    }			    
			    sASSERT(phys_neighbor_index != -1);
			    
			    edge_Saturations[last_vertex_id][phys_neighbor_index] += capacity;
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(last_nogood_var);
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(nogood_variable);
			}
			last_vertex_id = path_vertex_id;
			last_nogood_var = nogood_variable;
		    }
		    ++neigh_index;
		}
	    }
	}

	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sInt_32 phys_neigh_index = 0;
	    //printf("phys v: %d:  ", phys_v_id);
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		//printf("%.3f (%.3f) ", edge_Saturations[phys_v_id][phys_neigh_index], (*phys_neighbor)->m_capacity);
		if (edge_Saturations[phys_v_id][phys_neigh_index] > (*phys_neighbor)->m_capacity + s_EPSILON)
		{
		    refined = true;		    
		    encoder->cast_BigMutex(solver, edge_Nogoods[phys_v_id][phys_neigh_index]);
		    //printf("\nNOGOOD GENE:%d\n", edge_Nogoods[phys_v_id][phys_neigh_index].size());
		}
		++phys_neigh_index;
	    }
	    //printf("\n");
	}
	
	return refined;
    }


    bool sPathEmbeddingModel::refine_LazyFlatModel(sBoolEncoder *encoder, Glucose::Solver *solver, const Mappings_vector &virtual_Embeddings, const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings)
    {
	bool refined = false;
	
	vector<sDouble> vertex_Saturations;
	vector<sBoolEncoder::VariableIDs_vector> vertex_Nogoods;

	typedef vector<sBoolEncoder::VariableIDs_vector> EdgeNogood_vector;
	typedef vector<EdgeNogood_vector> EdgeNogoods_vector;
	EdgeNogoods_vector edge_Nogoods;

	typedef vector<sDouble> Saturations_vector;
	typedef vector<Saturations_vector> EdgeSaturations_vector;
	EdgeSaturations_vector edge_Saturations;

	edge_Saturations.resize(m_physical_Network.get_VertexCount());
	edge_Nogoods.resize(m_physical_Network.get_VertexCount());	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    edge_Saturations[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size(), 0.0);
	    edge_Nogoods[phys_v_id].resize(m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.size());	    
	}
	
	vertex_Saturations.resize(m_physical_Network.get_VertexCount(), 0.0);
	vertex_Nogoods.resize(m_physical_Network.get_VertexCount());

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
//		printf("%d <=+ %.3f\n", virtual_Embeddings[vn_id][virt_v_id], m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity);
		
		vertex_Saturations[virtual_Embeddings[vn_id][virt_v_id]] += m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity;
//		printf("  %d,%d,%d\n", vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]);
		vertex_Nogoods[virtual_Embeddings[vn_id][virt_v_id]].push_back(calc_FlatVertexEmbeddingBitVariableID(vn_id, virt_v_id, virtual_Embeddings[vn_id][virt_v_id]));
	    }	    
	}	
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
//	    printf("Saturs %d: %.3f\n", phys_v_id, vertex_Saturations[phys_v_id]);
	    if (vertex_Saturations[phys_v_id] > m_physical_Network.m_Vertices[phys_v_id].m_capacity + s_EPSILON)
	    {
		refined = true;
/*
		printf("refining...\n");
		for (sInt_32 var_id = 0; var_id < vertex_Nogoods[phys_v_id].size(); ++var_id)
		{
		    printf("    %d\n", vertex_Nogoods[phys_v_id][var_id]);
		}
*/
		encoder->cast_BigMutex(solver, vertex_Nogoods[phys_v_id]);
//		encoder->cast_Mutex(solver, vertex_Nogoods[phys_v_id][0], vertex_Nogoods[phys_v_id][1]);
	    }
	}
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
//	    m_virtual_Networks[vn_id].to_Screen();    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 last_vertex_id = -1;
//		    sInt_32 last_nogood_var = -1;
		    for (sInt_32 path_index = 0; path_index < path_Embeddings[vn_id][virt_v_id][neigh_index].size(); ++path_index)
//		    for (sInt_32 path_index = 0; path_index < m_virtual_Networks[vn_id].get_VertexCount(); ++path_index)
		    {
			sDouble capacity = (*virt_neighbor)->m_capacity;
			sInt_32 path_vertex_id = path_Embeddings[vn_id][virt_v_id][neigh_index][path_index];

			//sInt_32 nogood_variable = calc_FlatEdgeEmbeddingBitVariableID(vn_id, virt_v_id, neigh_index, path_index, path_vertex_id);
			
			if (last_vertex_id >= 0 && path_vertex_id >= 0)
			{			    
			    sInt_32 phys_neighbor_index = -1, phys_neigh_index = 0;
				
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
			    {
				if ((*phys_neighbor)->m_target->m_id == path_vertex_id)
				{
				    phys_neighbor_index = phys_neigh_index;
				    break;
				}
				++phys_neigh_index;
			    }
			    sASSERT(phys_neighbor_index != -1);

			    sInt_32 nogood_variable = calc_FlatEdgeNeighborEmbeddingBitVariableID(vn_id, virt_v_id, neigh_index, last_vertex_id, phys_neighbor_index);

			    edge_Saturations[last_vertex_id][phys_neighbor_index] += capacity;
			    //edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(last_nogood_var);
			    edge_Nogoods[last_vertex_id][phys_neighbor_index].push_back(nogood_variable);
			}
			last_vertex_id = path_vertex_id;
//			last_nogood_var = nogood_variable;			    
		    }
		    ++neigh_index;
		}
	    }
	}
	
	for (sInt_32 phys_v_id = 0; phys_v_id < m_physical_Network.get_VertexCount(); ++phys_v_id)
	{
	    sInt_32 phys_neigh_index = 0;
	    //printf("phys v: %d:  ", phys_v_id);
	    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_v_id].m_out_Neighbors.end(); ++phys_neighbor)
	    {
		//printf("%.3f (%.3f) ", edge_Saturations[phys_v_id][phys_neigh_index], (*phys_neighbor)->m_capacity);		
		if (edge_Saturations[phys_v_id][phys_neigh_index] > (*phys_neighbor)->m_capacity + s_EPSILON)
		{
		    refined = true;		    
		    encoder->cast_BigMutex(solver, edge_Nogoods[phys_v_id][phys_neigh_index]);
		    //printf("\nNOGOOD GENE:%ld\n", edge_Nogoods[phys_v_id][phys_neigh_index].size());
		}
		++phys_neigh_index;
	    }
	    //printf("\n");
	}
	
	return refined;
    }    
    

    sInt_32 sPathEmbeddingModel::calc_VertexEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_v_id, sInt_32 phys_v_id) const
    {
	sASSERT(!m_vertex_mapping_Offsets.empty());
	
	return (m_vertex_mapping_Offsets[vnet_id] + virt_v_id * m_physical_Network.get_VertexCount() + phys_v_id);
    }

    
    sInt_32 sPathEmbeddingModel::calc_EdgeEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_u_id, sInt_32 phys_v_id) const
    {
	sASSERT(!m_edge_mapping_Offsets.empty());

	return (m_edge_mapping_Offsets[vnet_id][virt_u_id][neighbor_index] + phys_u_id * m_physical_Network.get_VertexCount() + phys_v_id);
    }


    sInt_32 sPathEmbeddingModel::calc_FlatVertexEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_v_id, sInt_32 phys_v_id) const
    {
	sASSERT(!m_vertex_mapping_Offsets.empty());
	
	return (m_vertex_mapping_Offsets[vnet_id] + virt_v_id * m_physical_Network.get_VertexCount() + phys_v_id);
    }

    
    sInt_32 sPathEmbeddingModel::calc_FlatEdgeVertexEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_v_id) const
    {
	sASSERT(!m_edge_mapping_Offsets.empty());

	return (m_edge_mapping_Offsets[vnet_id][virt_u_id][neighbor_index] + phys_v_id);
    }

    
    sInt_32 sPathEmbeddingModel::calc_FlatEdgeNeighborEmbeddingBitVariableID(sInt_32 vnet_id, sInt_32 virt_u_id, sInt_32 neighbor_index, sInt_32 phys_v_id, sInt_32 phys_neighbor_index) const
    {
	return (m_edge_neighbor_mapping_Offsets[vnet_id][virt_u_id][neighbor_index][phys_v_id] + phys_neighbor_index);	
    }
    

    sDouble sPathEmbeddingModel::calc_EmbeddingCost(sBoolEncoder *sUNUSED(encoder), Glucose::Solver *sUNUSED(solver), const Mappings_vector &sUNUSED(vertex_Embeddings), const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings, sInt_32 depth) const
    {
	sDouble cost = 0.0;

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		cost += m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity;
	    }	    
	}	
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    //m_virtual_Networks[vn_id].to_Screen();
	    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 last_vertex_id = -1;
		    for (sInt_32 path_index = 0; path_index < depth; ++path_index)
//		    for (sInt_32 path_index = 0; path_index < m_virtual_Networks[vn_id].get_VertexCount(); ++path_index)
		    {			
			sDouble capacity = (*virt_neighbor)->m_capacity;
			sInt_32 path_vertex_id = path_Embeddings[vn_id][virt_v_id][neigh_index][path_index];

			if (last_vertex_id >= 0 && path_vertex_id >= 0)
			{
			    sInt_32 phys_neighbor_index = -1, phys_neigh_index = 0;
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
			    {
				if ((*phys_neighbor)->m_target->m_id == path_vertex_id)
				{
				    phys_neighbor_index = phys_neigh_index;
				    break;
				}
				++phys_neigh_index;
			    }			    
			    sASSERT(phys_neighbor_index != -1);

			    cost += capacity;
			}
			last_vertex_id = path_vertex_id;
		    }
		    ++neigh_index;
		}
	    }
	}
	
	return cost;	
    }


    sDouble sPathEmbeddingModel::calc_EmbeddingCost(sBoolEncoder *sUNUSED(encoder), Glucose::Solver *sUNUSED(solver), const Mappings_vector &sUNUSED(vertex_Embeddings), const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings) const
    {
	sDouble cost = 0.0;

	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		cost += m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_capacity;
	    }	    
	}	
	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    //m_virtual_Networks[vn_id].to_Screen();
	    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;

		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 last_vertex_id = -1;

		    /*
		    printf("Path:\n");
		    for (sInt_32 pi = 0; pi < path_Embeddings[vn_id][virt_v_id][neigh_index].size(); ++pi)
		    {
			printf("%d ", path_Embeddings[vn_id][virt_v_id][neigh_index][pi]);
		    }
		    printf("\n");
		    */
		    
		    for (sInt_32 path_index = 0; path_index < path_Embeddings[vn_id][virt_v_id][neigh_index].size(); ++path_index)
		    {			
			sDouble capacity = (*virt_neighbor)->m_capacity;
			sInt_32 path_vertex_id = path_Embeddings[vn_id][virt_v_id][neigh_index][path_index];

			if (last_vertex_id >= 0 && path_vertex_id >= 0)
			{
			    sInt_32 phys_neighbor_index = -1, phys_neigh_index = 0;
			    //printf("vn:%d, virtv:%d, neigh:%d LI:%d PI: %d PV:%d\n", vn_id, virt_v_id, neigh_index, last_vertex_id, path_index, path_vertex_id);
			    for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[last_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
			    {
				if ((*phys_neighbor)->m_target->m_id == path_vertex_id)
				{
				    phys_neighbor_index = phys_neigh_index;
				    break;
				}
				++phys_neigh_index;
			    }			    
			    sASSERT(phys_neighbor_index != -1);

			    cost += capacity;
			}
			last_vertex_id = path_vertex_id;
		    }
		    ++neigh_index;
		}
	    }
	}
	
	return cost;	
    }    

    
    void sPathEmbeddingModel::decode_VertexEmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &virt_v_id, sInt_32 &phys_v_id) const
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    sInt_32 first_ID = m_vertex_mapping_Offsets[vn_id];
	    sInt_32 last_ID = first_ID + m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();

	    if (variable_ID >= first_ID && variable_ID < last_ID)
	    {
		vnet_id = vn_id;
		sInt_32 remainder = variable_ID - first_ID;

		virt_v_id = remainder / m_physical_Network.get_VertexCount();
		phys_v_id = remainder % m_physical_Network.get_VertexCount();
		
		return;
	    }
	}	
    }


    void sPathEmbeddingModel::decode_EdgeEmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &u_id, sInt_32 &v_id, sInt_32 &neighbor_index, sInt_32 &phys_u_id, sInt_32 &path_index) const
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    sInt_32 vnet_size = m_physical_Network.get_VertexCount() * m_physical_Network.get_VertexCount();
	    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		
		    if (variable_ID >= m_edge_mapping_Offsets[vn_id][virt_v_id][neigh_index] && variable_ID < m_edge_mapping_Offsets[vn_id][virt_v_id][neigh_index] + vnet_size)
		    {
			sInt_32 subindex = variable_ID - m_edge_mapping_Offsets[vn_id][virt_v_id][neigh_index];
			
			path_index = subindex / m_physical_Network.get_VertexCount();
			phys_u_id = subindex % m_physical_Network.get_VertexCount();

			vnet_id = vn_id;
			u_id = virt_v_id;
			v_id = (*virt_neighbor)->m_target->m_id;
			neighbor_index = neigh_index;

			return;
		    }
		    ++neigh_index;
		}
	    }
	}
    }


    void sPathEmbeddingModel::decode_LimitedEdgeEmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &u_id, sInt_32 &v_id, sInt_32 &neighbor_index, sInt_32 &phys_u_id, sInt_32 &path_index, sInt_32 depth) const
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    sInt_32 vnet_size = m_physical_Network.get_VertexCount() * depth;
	    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		
		    if (variable_ID >= m_edge_mapping_Offsets[vn_id][virt_v_id][neigh_index] && variable_ID < m_edge_mapping_Offsets[vn_id][virt_v_id][neigh_index] + vnet_size)
		    {
			sInt_32 subindex = variable_ID - m_edge_mapping_Offsets[vn_id][virt_v_id][neigh_index];
			
			path_index = subindex / m_physical_Network.get_VertexCount();
			phys_u_id = subindex % m_physical_Network.get_VertexCount();

			vnet_id = vn_id;
			u_id = virt_v_id;
			v_id = (*virt_neighbor)->m_target->m_id;
			neighbor_index = neigh_index;

			return;
		    }
		    ++neigh_index;
		}
	    }
	}
    }


    void sPathEmbeddingModel::decode_FlatVertexEmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &virt_v_id, sInt_32 &phys_v_id) const
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    sInt_32 first_ID = m_vertex_mapping_Offsets[vn_id];
	    sInt_32 last_ID = first_ID + m_virtual_Networks[vn_id].get_VertexCount() * m_physical_Network.get_VertexCount();

	    if (variable_ID >= first_ID && variable_ID < last_ID)
	    {
		vnet_id = vn_id;
		sInt_32 remainder = variable_ID - first_ID;

		virt_v_id = remainder / m_physical_Network.get_VertexCount();
		phys_v_id = remainder % m_physical_Network.get_VertexCount();
		
		return;
	    }
	}	
    }


    void sPathEmbeddingModel::decode_FlatEdgeEmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &u_id, sInt_32 &v_id, sInt_32 &neighbor_index, sInt_32 &phys_u_id) const
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    sInt_32 vnet_size = m_physical_Network.get_VertexCount();
	    
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{		
		    if (variable_ID >= m_edge_mapping_Offsets[vn_id][virt_v_id][neigh_index] && variable_ID < m_edge_mapping_Offsets[vn_id][virt_v_id][neigh_index] + vnet_size)
		    {
			sInt_32 subindex = variable_ID - m_edge_mapping_Offsets[vn_id][virt_v_id][neigh_index];
			
			phys_u_id = subindex % m_physical_Network.get_VertexCount();

			vnet_id = vn_id;
			u_id = virt_v_id;
			v_id = (*virt_neighbor)->m_target->m_id;
			neighbor_index = neigh_index;

			return;
		    }
		    ++neigh_index;
		}
	    }
	}
    }    


    void sPathEmbeddingModel::decode_FlatEdgeNeighborEmbeddingMapping(sInt_32 variable_ID, sInt_32 &vnet_id, sInt_32 &u_id, sInt_32 &v_id, sInt_32 &neighbor_index, sInt_32 &phys_u_id, sInt_32 &phys_neighbor_index) const
    {
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    for (sInt_32 virt_v_id = 0; virt_v_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_v_id)
	    {
		sInt_32 neigh_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_v_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    for (sInt_32 phys_vertex_id = 0; phys_vertex_id < m_physical_Network.get_VertexCount(); ++phys_vertex_id)
		    {		    
			//sInt_32 phys_neigh_index = 0;		    
			//for (s_Vertex::Neighbors_vector::const_iterator phys_neighbor = m_physical_Network.m_Vertices[phys_vertex_id].m_out_Neighbors.begin(); phys_neighbor != m_physical_Network.m_Vertices[phys_vertex_id].m_out_Neighbors.end(); ++phys_neighbor)
			//{
			//printf("Range: %d, %d\n", m_edge_neighbor_mapping_Offsets[vn_id][virt_v_id][neigh_index][phys_vertex_id], m_edge_neighbor_mapping_Offsets[vn_id][virt_v_id][neigh_index][phys_vertex_id] + m_physical_Network.m_Vertices[phys_vertex_id].m_out_Neighbors.size());
			if (   variable_ID >= m_edge_neighbor_mapping_Offsets[vn_id][virt_v_id][neigh_index][phys_vertex_id]
			    && variable_ID < m_edge_neighbor_mapping_Offsets[vn_id][virt_v_id][neigh_index][phys_vertex_id] + m_physical_Network.m_Vertices[phys_vertex_id].m_out_Neighbors.size())
			{
			    sInt_32 subindex = variable_ID - m_edge_neighbor_mapping_Offsets[vn_id][virt_v_id][neigh_index][phys_vertex_id];
			    
			    phys_neighbor_index = subindex;
			    vnet_id = vn_id;
			    u_id = virt_v_id;
			    v_id = (*virt_neighbor)->m_target->m_id;
			    neighbor_index = neigh_index;
			    phys_u_id = phys_vertex_id;

			    /*
			    printf("Variable: %d\n", variable_ID);
			    printf("phys_u_id: %d, vnet_id: %d, u_id: %d, v_id: %d, neighbor_index: %d, phys_neighbor_index: %d\n",
				   phys_u_id,
				   vnet_id,
				   u_id,
				   v_id,
				   neighbor_index,
				   phys_neighbor_index);
			    */
			    return;
			}
			//++phys_neigh_index;
		    }
		    ++neigh_index;
		}
	    }
	}	
    }
    /*
	for (sInt_32 i = 0; i < solver->nVars(); i++)
	{
	    sInt_32 literal;
		    
	    if (solver->model[i] != l_Undef)
	    {
		literal = (solver->model[i] == l_True) ? i + 1 : -(i+1);
	    }
	    else
	    {
		sASSERT(false);
	    }

	    if (literal > 0)
	    {
		sInt_32 variable_ID = sABS(literal);
    
    */


    /*
    */
    
/*----------------------------------------------------------------------------*/
    
    void sPathEmbeddingModel::to_Screen(const sString &indent) const
    {
	to_Stream(stdout, indent);
    }

    
    void sPathEmbeddingModel::to_Stream(FILE *fw, const sString &indent) const
    {
	fprintf(fw, "%sPath Embedding Model: {\n", indent.c_str());
	
	fprintf(fw, "%s%sphysical network:\n", indent.c_str(), s_INDENT.c_str());
	m_physical_Network.to_Screen(indent + s2_INDENT);

	fprintf(fw, "%s%svirtual networks: {\n", indent.c_str(), s_INDENT.c_str());	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    m_virtual_Networks[vn_id].to_Screen(indent + s2_INDENT);
	}
	fprintf(fw, "%s%s}\n", indent.c_str(), s_INDENT.c_str());		

	fprintf(fw, "%s%svertex mapping offsets: {\n", indent.c_str(), s_INDENT.c_str());		
	for (VertexOffsets_vector::const_iterator mapping_offset = m_vertex_mapping_Offsets.begin(); mapping_offset != m_vertex_mapping_Offsets.end(); ++mapping_offset)
	{
	    fprintf(fw, "%s%s%d\n", indent.c_str(), s2_INDENT.c_str(), *mapping_offset);
	}
	fprintf(fw, "%s%s}\n", indent.c_str(), s_INDENT.c_str());
	fprintf(fw, "%s%sedge mapping offsets: {\n", indent.c_str(), s_INDENT.c_str());
	sInt_32 vn = 0;
	for (EdgeOffsets_vector::const_iterator vn_mapping_offsets = m_edge_mapping_Offsets.begin(); vn_mapping_offsets != m_edge_mapping_Offsets.end(); ++vn_mapping_offsets)
	{
	    fprintf(fw, "%s%s%svn: %d {\n", indent.c_str(), s_INDENT.c_str(), s_INDENT.c_str(), vn);

	    sInt_32 vertex = 0;
	    for (Offsets_2vector::const_iterator edge_mapping_offsets = vn_mapping_offsets->begin(); edge_mapping_offsets != vn_mapping_offsets->end(); ++edge_mapping_offsets)
	    {
		fprintf(fw, "%s%s%s%svertex: %d {\n", indent.c_str(), s_INDENT.c_str(), s_INDENT.c_str(), s_INDENT.c_str(), vertex);

		sInt_32 neighbor = 0;
		for (Offsets_vector::const_iterator edge_mapping_offset = edge_mapping_offsets->begin(); edge_mapping_offset != edge_mapping_offsets->end(); ++edge_mapping_offset)
		{
		    fprintf(fw, "%s%sneighbor %d: %d\n", indent.c_str(), s4_INDENT.c_str(), neighbor, *edge_mapping_offset);
		    ++neighbor;
		}
		fprintf(fw, "%s%s%s%s}\n", indent.c_str(), s_INDENT.c_str(), s_INDENT.c_str(), s_INDENT.c_str());
		++vertex;
	    }
	    fprintf(fw, "%s%s%s}\n", indent.c_str(), s_INDENT.c_str(), s_INDENT.c_str());
	    ++vn;
	}	
	fprintf(fw, "%s%s}\n", indent.c_str(), s_INDENT.c_str());
	fprintf(fw, "%s}\n", indent.c_str());	
    }


    void sPathEmbeddingModel::to_Screen_embedding(const Mappings_vector &vertex_Embeddings, const sPathEmbeddingModel::NetworkPathMappings_vector &path_Embeddings, const sString &indent) const
    {	
	for (sInt_32 vn_id = 0; vn_id < m_virtual_Networks.size(); ++vn_id)
	{
	    printf("%sVirtual network: %d\n", indent.c_str(), vn_id);

	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		printf("%s%s%d --> %d\n", indent.c_str(), s_INDENT.c_str(), virt_u_id, vertex_Embeddings[vn_id][virt_u_id]);
	    }

	    for (sInt_32 virt_u_id = 0; virt_u_id < m_virtual_Networks[vn_id].get_VertexCount(); ++virt_u_id)
	    {
		sInt_32 neighbor_index = 0;
		for (s_Vertex::Neighbors_vector::const_iterator virt_neighbor = m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.begin(); virt_neighbor != m_virtual_Networks[vn_id].m_Vertices[virt_u_id].m_out_Neighbors.end(); ++virt_neighbor)
		{
		    sInt_32 neighbor_id = (*virt_neighbor)->m_target->m_id;
		    printf("%s%s%d (%d) -- %d (%d):\n", indent.c_str(), s_INDENT.c_str(), virt_u_id, vertex_Embeddings[vn_id][virt_u_id], neighbor_id, vertex_Embeddings[vn_id][neighbor_id]);

		    printf("%s%s%s", indent.c_str(), s_INDENT.c_str(), s_INDENT.c_str());
		    for (sInt_32 path_index = 0; path_index < path_Embeddings[vn_id][virt_u_id][neighbor_index].size(); ++path_index)
		    {
			sInt_32 phys_u_id = path_Embeddings[vn_id][virt_u_id][neighbor_index][path_index];

			if (phys_u_id >= 0)
			{
			    printf("%d ", phys_u_id);
			}
			else
			{
			    break;
			}
		    }
		    printf("\n");
		    ++neighbor_index;
		}
	    }
	}	
    }
    
    

    
    
/*----------------------------------------------------------------------------*/

} // namespace realX
