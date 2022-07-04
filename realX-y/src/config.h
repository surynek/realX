/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-055_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* config.h / 0-055_nofutu                                                    */
/*----------------------------------------------------------------------------*/
//
// Configuration file for auRIx package - global settings.
//
/*----------------------------------------------------------------------------*/

#ifndef __CONFIG_H__
#define __CONFIG_H__


/*----------------------------------------------------------------------------*/

namespace realX
{


/*----------------------------------------------------------------------------*/

#define s__STANDARD_INDENT                "    "
    
#define s__DEFAULT_N_PARALLEL_THREADS          4
#define s__DEFAULT_RANDOM_WALK_LENGTH    1048576

#define s__DEFAULT_WRAP_LINE_LENGTH           80
#define s__CONVERSION_BUFFER_SIZE            128




/*----------------------------------------------------------------------------*/

} // namespace realX

#endif /* __CONFIG_H__ */
