/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-104_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* linpath_test.h / 0-104_nofutu                                              */
/*----------------------------------------------------------------------------*/
//
// Path planning in presence of obstacles based on linear programming.
//
/*----------------------------------------------------------------------------*/


#ifndef __LINPATH_TEST_H__
#define __LINPATH_TEST_H__

#include "defs.h"
#include "result.h"


/*----------------------------------------------------------------------------*/

namespace realX
{

void print_Introduction(void);

sResult test_linpath_1(void);
sResult test_linpath_2(void);


/*----------------------------------------------------------------------------*/

} // namespace realX

#endif /* __LINPATH_TEST_H__ */

