/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-107_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* embedding_test.h / 0-107_nofutu                                            */
/*----------------------------------------------------------------------------*/
//
// Virtual network embedding test.
//
/*----------------------------------------------------------------------------*/


#ifndef __EMBEDDING_TEST_H__
#define __EMBEDDING_TEST_H__

#include "defs.h"
#include "result.h"


/*----------------------------------------------------------------------------*/

namespace realX
{

void print_Introduction(void);

sResult test_embedding_1(void);
sResult test_embedding_2(void);
sResult test_embedding_3(void);
sResult test_embedding_4(void);
sResult test_embedding_5(void);


/*----------------------------------------------------------------------------*/

} // namespace realX

#endif /* __EMBEDDING_TEST_H__ */

