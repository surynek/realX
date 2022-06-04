/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-052_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* linpath_test.cpp / 0-052_nofutu                                            */
/*----------------------------------------------------------------------------*/
//
// Path planning in presence of obstacles based on linear programming.
//
/*----------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include <glpk.h>

#include "defs.h"
#include "compile.h"
#include "result.h"
#include "version.h"

#include "common/types.h"
#include "common/graph.h"
#include "core/robot.h"
#include "util/geometry.h"

#include "test/linpath_test.h"


using namespace std;
using namespace realX;


/*----------------------------------------------------------------------------*/

namespace realX
{




/*----------------------------------------------------------------------------*/

void print_Introduction(void)
{
    printf("----------------------------------------------------------------\n");    
    printf("%s : LinPath Test Program\n", sPRODUCT);
    printf("%s\n", sCOPYRIGHT);
    printf("================================================================\n");
}


sResult linpath_test_1(void)
{
    glp_prob *lp;
    
    int ia[1024], ja[1024];    
    double ar[1024];
    
    double z, t0, t1, t2, t3, w01, w02, w13, w23;

    printf("Testing GLPK 1\n");
    printf("\n");
    
    lp = glp_create_prob();    
    
    glp_set_prob_name(lp, "LP-path");
    glp_set_obj_dir(lp, GLP_MAX);
    
    glp_add_rows(lp, 4);
    glp_set_row_name(lp, 1, "F1");
    glp_set_row_bnds(lp, 1, GLP_LO, -2.0, 100.0);    
    glp_set_row_name(lp, 2, "F2");
    glp_set_row_bnds(lp, 2, GLP_LO, -3.0, 100.0);
    glp_set_row_name(lp, 3, "F3");
    glp_set_row_bnds(lp, 3, GLP_LO, -3.0, 100.0);
    glp_set_row_name(lp, 4, "F4");
    glp_set_row_bnds(lp, 4, GLP_LO, -3.0, 100.0);    
    
    glp_add_cols(lp, 8);
    glp_set_col_name(lp, 1, "t0");
    glp_set_col_bnds(lp, 1, GLP_UP, 0.0, 0.0);
    glp_set_obj_coef(lp, 1, 1.0);

    glp_set_col_name(lp, 2, "t1");
    glp_set_col_bnds(lp, 2, GLP_LO, 0.0, 0.0);
    glp_set_obj_coef(lp, 2, 1.0);

    glp_set_col_name(lp, 3, "t2");
    glp_set_col_bnds(lp, 3, GLP_LO, 2.0, 0.0); // safe lower bound
    glp_set_obj_coef(lp, 3, 1.0);

    glp_set_col_name(lp, 4, "t3");
    glp_set_col_bnds(lp, 4, GLP_LO, 3.0, 0.0); // safe lower bound
    glp_set_obj_coef(lp, 4, 1.0);

    glp_set_col_name(lp, 5, "w01");
    glp_set_col_bnds(lp, 5, GLP_LO, 0.0, 0.0);
    glp_set_obj_coef(lp, 5, -10.0);
    glp_set_col_name(lp, 6, "w02");
    glp_set_col_bnds(lp, 6, GLP_LO, 0.0, 0.0);
    glp_set_obj_coef(lp, 6, -10.0);
    glp_set_col_name(lp, 7, "w13");
    glp_set_col_bnds(lp, 7, GLP_LO, 0.0, 0.0);
    glp_set_obj_coef(lp, 7, -10.0);
    glp_set_col_name(lp, 8, "w23");
    glp_set_col_bnds(lp, 8, GLP_LO, 0.0, 0.0);
    glp_set_obj_coef(lp, 8, -10.0);        
        
    
    ia[1] = 1, ja[1] = 1, ar[1] =  1.0;
    ia[2] = 1, ja[2] = 2, ar[2] =  -1.0;
    ia[3] = 1, ja[3] = 3, ar[3] =  0.0;
    ia[4] = 1, ja[4] = 4, ar[4] =  0.0;
    ia[5] = 1, ja[5] = 5, ar[5] =  1.0;
    ia[6] = 1, ja[6] = 6, ar[6] =  0.0;
    ia[7] = 1, ja[7] = 7, ar[7] =  0.0;
    ia[8] = 1, ja[8] = 8, ar[8] =  0.0;

    ia[9]  = 2, ja[9]  = 1, ar[9]  =  1.0;
    ia[10] = 2, ja[10] = 2, ar[10] =  0.0;
    ia[11] = 2, ja[11] = 3, ar[11] =  -1.0;
    ia[12] = 2, ja[12] = 4, ar[12] =  0.0;
    ia[13] = 2, ja[13] = 5, ar[13] =  0.0;
    ia[14] = 2, ja[14] = 6, ar[14] =  1.0;
    ia[15] = 2, ja[15] = 7, ar[15] =  0.0;
    ia[16] = 2, ja[16] = 8, ar[16] =  0.0;

    ia[17] = 3, ja[17] = 1, ar[17] =  0.0;
    ia[18] = 3, ja[18] = 2, ar[18] =  1.0;
    ia[19] = 3, ja[19] = 3, ar[19] =  0.0;
    ia[20] = 3, ja[20] = 4, ar[20] =  -1.0;
    ia[21] = 3, ja[21] = 5, ar[21] =  0.0;
    ia[22] = 3, ja[22] = 6, ar[22] =  0.0;
    ia[23] = 3, ja[23] = 7, ar[23] =  1.0;
    ia[24] = 3, ja[24] = 8, ar[24] =  0.0;
 
    ia[25] = 4, ja[25] = 1, ar[25] =  0.0;
    ia[26] = 4, ja[26] = 2, ar[26] =  0.0;
    ia[27] = 4, ja[27] = 3, ar[27] =  1.0;
    ia[28] = 4, ja[28] = 4, ar[28] =  -1.0;
    ia[29] = 4, ja[29] = 5, ar[29] =  0.0;
    ia[30] = 4, ja[30] = 6, ar[30] =  0.0;
    ia[31] = 4, ja[31] = 7, ar[31] =  0.0;
    ia[32] = 4, ja[32] = 8, ar[32] =  1.0;               
    
    glp_load_matrix(lp, 32, ia, ja, ar);

    glp_simplex(lp, NULL);
    z = glp_get_obj_val(lp);

    t0 = glp_get_col_prim(lp, 1);
    t1 = glp_get_col_prim(lp, 2);
    t2 = glp_get_col_prim(lp, 3);
    t3 = glp_get_col_prim(lp, 4);

    w01 = glp_get_col_prim(lp, 5);
    w02 = glp_get_col_prim(lp, 6);
    w13 = glp_get_col_prim(lp, 7);
    w23 = glp_get_col_prim(lp, 8);        

    printf("Objective = %.3f\n", z);
    printf("t0  = %.3f, t1  = %.3f; t2  = %.3f; t3  = %.3f\n", t0, t1, t2, t3);
    printf("w01 = %.3f, w02 = %.3f; w13 = %.3f; w23 = %.3f\n", w01, w02, w13, w23);    
    glp_delete_prob(lp);

    return sRESULT_SUCCESS;
}


sResult linpath_test_2(void)
{
//    glp_prob *lp;
    
    printf("Testing GLPK 1\n");
    printf("\n");

    s_DirectedGraph directed_graph;
    directed_graph.add_Vertices(4);

    directed_graph.add_Arrow(0, 1, 2.0);
    directed_graph.add_Arrow(1, 3, 3.0);
    directed_graph.add_Arrow(0, 2, 3.0);
    directed_graph.add_Arrow(2, 3, 3.0);

    directed_graph.to_Screen();

    glp_prob *lp;    
    lp = glp_create_prob();    
    
    glp_set_prob_name(lp, "LP-path");
    glp_set_obj_dir(lp, GLP_MAX);

    
    sInt_32 matrix_size = 16384;
    
    sInt_32 *matrix_Is = new sInt_32[matrix_size];
    sInt_32 *matrix_Js = new sInt_32[matrix_size];
    sDouble *matrix_As = new sDouble[matrix_size];

    glp_add_rows(lp, directed_graph.get_ArcCount());

    sInt_32 row_size = directed_graph.get_VertexCount() + directed_graph.get_ArcCount();

    sInt_32 curr_off = 0;
    sInt_32 row = 0;

    glp_add_cols(lp, row_size);
	
    for (sInt_32 j = 0; j < directed_graph.get_VertexCount(); ++j)
    {
	sString t_str = sInt_32_to_String(j);
		
	glp_set_col_name(lp, j, ("t" + t_str).c_str());
	glp_set_col_bnds(lp, j, GLP_UP, 0.0, 0.0);
	glp_set_obj_coef(lp, j, 1.0);	
    }

    sInt_32 j = directed_graph.get_VertexCount();   
    for (s_DirectedGraph::Arcs_list::const_iterator arc = directed_graph.m_Arcs.begin(); arc != directed_graph.m_Arcs.end(); ++arc)
    {    
	sString wi_str = sInt_32_to_String(arc->m_source->m_id);
	sString wj_str = sInt_32_to_String(arc->m_target->m_id);

	glp_set_col_name(lp, j, ("w" + wi_str + wj_str).c_str());
	glp_set_col_bnds(lp, j, GLP_LO, 0.0, 0.0);
	glp_set_obj_coef(lp, j, -10.0);	
	
	++j;	
    }
    	     
    
    for (s_DirectedGraph::Arcs_list::const_iterator arc = directed_graph.m_Arcs.begin(); arc != directed_graph.m_Arcs.end(); ++arc)
    {
	sString row_str = sInt_32_to_String(row);
	
	glp_set_row_name(lp, row, ("F" + row_str).c_str());
	glp_set_row_bnds(lp, 1, GLP_LO, -arc->m_capacity, 100.0);        
	
	for (sInt_32 j = 0; j < row_size; ++j)
	{
	    matrix_Is[curr_off + j] = row;
	    matrix_Js[curr_off + j] = j;
	    matrix_As[curr_off + j] = 0.0;
	}

	sInt_32 off_ti = arc->m_source->m_id;
	sInt_32 off_tj = arc->m_target->m_id;
	sInt_32 off_wij = directed_graph.get_VertexCount() + arc->m_id;	

	matrix_Is[curr_off + off_ti] = row;
	matrix_Js[curr_off + off_ti] = off_ti;
	matrix_As[curr_off + off_ti] = 1.0;

	matrix_Is[curr_off + off_tj] = row;
	matrix_Js[curr_off + off_tj] = off_ti;
	matrix_As[curr_off + off_tj] = -1.0;

	matrix_Is[curr_off + off_wij] = row;
	matrix_Js[curr_off + off_wij] = off_wij;
	matrix_As[curr_off + off_wij] = 1.0;	

	curr_off += row_size;
	++row;
    }

    delete matrix_Is;
    delete matrix_Js;
    delete matrix_As;
    
    return sRESULT_SUCCESS;
}


}  // namespace realX


/*----------------------------------------------------------------------------*/

int main(int sUNUSED(argc), const char **sUNUSED(argv))
{
    sResult result;

    print_Introduction();

    if (sFAILED(result = linpath_test_1()))
    {
	printf("Test linpath 1 failed (error:%d).\n", result);
	return result;
    }

    if (sFAILED(result = linpath_test_2()))
    {
	printf("Test linpath 2 failed (error:%d).\n", result);
	return result;
    }
    
    return sRESULT_SUCCESS;
}
