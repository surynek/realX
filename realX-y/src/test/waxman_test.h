/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-066_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* waxman_test.h / 0-066_nofutu                                               */
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


/*----------------------------------------------------------------------------*/

} // namespace realX

#endif /* __WAXMAN_TEST_H__ */

