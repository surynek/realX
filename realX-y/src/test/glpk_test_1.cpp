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
/* glpk_test_1.cpp / 0-133_nofutu                                             */
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <glpk.h>

int main(void)
{
    glp_prob *lp;
    
    int ia[1024], ja[1024];    
    double ar[1024];
    
    double z, t0, t1, t2, t3, w01, w02, w13, w23;

    printf("Testing GLPK\n");
    printf("(c) 2022 Pavel Surynek\n");
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
    glp_set_col_bnds(lp, 3, GLP_LO, 2.0, 0.0);
    glp_set_obj_coef(lp, 3, 1.0);

    glp_set_col_name(lp, 4, "t3");
    glp_set_col_bnds(lp, 4, GLP_LO, 3.0, 0.0);
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
    
    return 0;
}
