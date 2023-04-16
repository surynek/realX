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
/* waxman_test.h / 0-125_nofutu                                               */
/*----------------------------------------------------------------------------*/
//
// Waxman graph testing for virtual network embedding.
//
/*----------------------------------------------------------------------------*/


#ifndef __WAXMAN_TEST_H__
#define __WAXMAN_TEST_H__

#include "defs.h"
#include "result.h"


/*----------------------------------------------------------------------------*/

namespace realX
{

void print_Introduction(void);

sResult test_waxman_1(void);
sResult test_waxman_2(void);
sResult test_waxman_3(void);
sResult test_waxman_4(void);

sResult test_waxman_5(void);
sResult waxman_test_5_geo_circ(void);
sResult waxman_test_5_inc_geo_circ(void);
sResult waxman_test_5_tree(void);
sResult waxman_test_5_tree_geo_circ(void);
sResult waxman_test_5_tree_inc_geo_circ(void);
sResult waxman_test_5_graph(void);
sResult waxman_test_5_graph_geo_circ(void);
sResult waxman_test_5_graph_inc_geo_circ(void);

sResult waxman_test_6(void);
sResult waxman_test_7(void);
sResult waxman_test_7_tree(void);
sResult waxman_test_7_graph(void);

sResult waxman_test_8(void);


/*----------------------------------------------------------------------------*/

} // namespace realX

#endif /* __WAXMAN_TEST_H__ */

