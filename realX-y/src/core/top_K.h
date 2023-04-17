/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-124_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* top_K.h / 0-124_nofutu                                                     */
/*----------------------------------------------------------------------------*/
//
// Top K path finding problem solvers.
//
/*----------------------------------------------------------------------------*/


#ifndef __TOP_K_H__
#define __TOP_K_H__


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
    
    class sTOP_K_PathModel
    {
    public:
	typedef std::vector<s_DirectedGraph> DirectedGraphs_vector;
	
	typedef std::vector<sInt_32> Offsets_vector;
	typedef std::vector<Offsets_vector> Offsets_2vector;
	typedef std::vector<Offsets_2vector> Offsets_3vector;	
	
	typedef Offsets_vector PathOffsets_vector;

    public:
	Glucose::Solver* setup_SATSolver(sDouble timeout = -1.0);
	void destroy_SATSolver(Glucose::Solver *solver);
	
	void setup_Basic_TopKModel(sBoolEncoder *encoder, sInt_32 path_length, sInt_32 K);
	void build_Basic_TopKModel(sBoolEncoder *encoder, Glucose::Solver *solver);
	
	bool solve_TopKModel(Glucose::Solver *solver);

    public:
	sInt_32 calc_PathBitVariableID(sInt_32 path_id, sInt_32 step_id, sInt_32 v_id) const;

    public:
	virtual void to_Screen(const sString &indent = "") const;
	virtual void to_Stream(FILE *fw, const sString &indent = "") const;

	virtual void to_Screen_TopK(const sString &indent = "") const;

    public:
	sInt_32 m_path_length;
	sInt_32 m_K;
	
	PathOffsets_vector m_path_Offsets;
	
	s_DirectedGraph m_Network;
    };    
  

/*----------------------------------------------------------------------------*/

} // namespace realX

#endif /* __TOP_K_H__ */
