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
/* io.h / 0-133_nofutu                                                        */
/*----------------------------------------------------------------------------*/
//
// Input/output functions and utilities.
//
/*----------------------------------------------------------------------------*/


#ifndef __IO_H__
#define __IO_H__

#include "result.h"


using namespace realX;


/*----------------------------------------------------------------------------*/

namespace realX
{




/*----------------------------------------------------------------------------*/
// Global functions

    sInt_32 sConsumeUntilChar(FILE *fr, sChar c);
    sInt_32 sConsumeUntilString(FILE *fr, const sString &string);
    
    sInt_32 sConsumeAlphaString(FILE *fr, sString &alpha_string);
    sInt_32 sConsumeAlnumString(FILE *fr, sString &alnum_string);    
    sInt_32 sConsumeNumericString(FILE *fr, sString &numeric_string);
    sInt_32 sConsumeFloatalString(FILE *fr, sString &floatal_string);
    sInt_32 sConsumeDigitalString(FILE *fr, sString &digital_string);    

    sInt_32 sConsumeWhiteSpaces(FILE *fr);
    

/*----------------------------------------------------------------------------*/

} // namespace realX

#endif /* __IO_H__ */
