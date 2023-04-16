/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-125_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* top_K.cpp / 0-125_nofutu                                                   */
/*----------------------------------------------------------------------------*/
//
// Robot (model) related data structures and functions.
//
/*----------------------------------------------------------------------------*/


#include <stdio.h>
#include <math.h>

#include "core/top_K.h"


using namespace realX;




/*----------------------------------------------------------------------------*/

namespace realX
{



    
/*============================================================================*/
// sTOP_K_PathModel class
/*----------------------------------------------------------------------------*/

    Glucose::Solver* sTOP_K_PathModel::setup_SATSolver(sDouble timeout)
    {
	Glucose::Solver *solver;
	solver = new Glucose::Solver;

	solver->s_Glucose_timeout = timeout;
	solver->setIncrementalMode();

	return solver;	
    }


    void sTOP_K_PathModel::destroy_SATSolver(Glucose::Solver *solver)
    {
	delete solver;
    }

    
    void sTOP_K_PathModel::setup_Basic_TopKModel(sBoolEncoder *encoder, sInt_32 path_length, sInt_32 K)
    {
	m_path_length = path_length;
	m_K = K;
	
	sInt_32 offset = 1;
	for (sInt_32 k = 0; k < K; ++k)
	{
	    m_path_Offsets.push_back(offset);
	    offset += path_length * m_Network.get_VertexCount();
	}	
	
	encoder->set_LastVariableID(offset);
    }


    void sTOP_K_PathModel::build_Basic_TopKModel(sBoolEncoder *encoder, Glucose::Solver *solver)
    {
	for (sInt_32 k = 0; k < m_K; ++k)
	{
	    for (sInt_32 l = 0; l < m_path_length; ++l)
	    {	
		for (sInt_32 v_id = 0; v_id < m_Network.get_VertexCount(); ++v_id)
		{
		    sInt_32 virt_to_phys_id = calc_PathBitVariableID(k, l, v_id);
		}
	    }
	}

	/*
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
	    else
	    {
		sInt_32 mapping_id = calc_EmbeddingBitVariableID(vn_id, virt_v_id, phys_v_id);
		encoder->cast_BitUnset(solver, mapping_id);
	    }
	}
	encoder->cast_Disjunction(solver, virt_to_phys_IDs);
	encoder->cast_AdaptiveAllMutexConstraint(solver, virt_to_phys_IDs);
	*/
    }

    
    bool sTOP_K_PathModel::solve_TopKModel(Glucose::Solver *solver)
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

    
/*----------------------------------------------------------------------------*/

    sInt_32 sTOP_K_PathModel::calc_PathBitVariableID(sInt_32 path_id, sInt_32 step_id, sInt_32 v_id) const
    {
	sASSERT(!m_path_Offsets.empty());
	
	return (m_path_Offsets[path_id] + step_id * m_Network.get_VertexCount() + v_id);
    }
    

/*----------------------------------------------------------------------------*/
    
    void sTOP_K_PathModel::to_Screen(const sString &indent) const
    {
	to_Stream(stdout, indent);
    }

    
    void sTOP_K_PathModel::to_Stream(FILE *fw, const sString &indent) const
    {
	fprintf(fw, "%sTop K Path Model: {\n", indent.c_str());	
	fprintf(fw, "%s}\n", indent.c_str());	
    }


    void sTOP_K_PathModel::to_Screen_TopK(const sString &indent) const
    {	
	printf("%s\n", indent.c_str());
    }
    
    
    
    
/*----------------------------------------------------------------------------*/

} // namespace realX
