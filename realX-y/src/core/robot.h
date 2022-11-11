/*============================================================================*/
/*                                                                            */
/*                                                                            */
<<<<<<< HEAD
/*                             realX 0-107_nofutu                             */
=======
/*                             realX 0-104_nofutu                             */
>>>>>>> 14f14b75c8f1d7815a0fb722e1fd0403f95fb68c
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
<<<<<<< HEAD
/* robot.h / 0-107_nofutu                                                     */
=======
/* robot.h / 0-104_nofutu                                                     */
>>>>>>> 14f14b75c8f1d7815a0fb722e1fd0403f95fb68c
/*----------------------------------------------------------------------------*/
//
// Robot (model) related data structures and functions.
//
/*----------------------------------------------------------------------------*/


#ifndef __ROBOT_H__
#define __ROBOT_H__


#include "common/types.h"


using namespace realX;


/*----------------------------------------------------------------------------*/

namespace realX
{
    class s2D
    {
    public:
	s2D() { /* nothing */ }
	s2D(sDouble _x, sDouble _y)
	    : x(_x), y(_y)
	{
	    // nothing
	}

    public:
	virtual void to_Screen(const sString &indent = "") const;
	virtual void to_Stream(FILE *fw, const sString &indent = "") const;

    public:
	sDouble x, y;
	
    };


    class s3D
    {
    public:
	s3D() { /* nothing */ }
	s3D(sDouble _x, sDouble _y, sDouble _z)
	    : x(_x), y(_y), z(_z)
	{
	    // nothing
	}

    public:
	virtual void to_Screen(const sString &indent = "") const;
	virtual void to_Stream(FILE *fw, const sString &indent = "") const;	

    public:
	sDouble x, y, z;
    };
    

    class sRobot
    {
    public:	    
    public:
    };


    class s2DRobot
	: public sRobot
    {
    public:
	struct Arm
	{
	    Arm(sDouble _length, sDouble _rotation)
		: length(_length), rotation(_rotation)
	    {
		// nothing
	    }
	    
	    sDouble length;	    
	    sDouble rotation;

	public:
	    virtual void to_Screen(const sString &indent = "") const;
	    virtual void to_Stream(FILE *fw, const sString &indent = "") const;		    
	};
	
    public:
	void calc_EndPosition(const Arm &arm, const s2D &origin, s2D &end) const;

    public:
    };
  

/*----------------------------------------------------------------------------*/

} // namespace realX

#endif /* __ROBOT_H__ */
