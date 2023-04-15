/*============================================================================*/
/*                                                                            */
/*                                                                            */
/*                             realX 0-122_nofutu                             */
/*                                                                            */
/*                  (C) Copyright 2021 - 2022 Pavel Surynek                   */
/*                                                                            */
/*                http://www.surynek.net | <pavel@surynek.net>                */
/*       http://users.fit.cvut.cz/surynek | <pavel.surynek@fit.cvut.cz>       */
/*                                                                            */
/*============================================================================*/
/* robot.cpp / 0-122_nofutu                                                   */
/*----------------------------------------------------------------------------*/
//
// Robot (model) related data structures and functions.
//
/*----------------------------------------------------------------------------*/


#include <stdio.h>
#include <math.h>

#include "core/robot.h"


using namespace realX;




/*----------------------------------------------------------------------------*/

namespace realX
{



    
/*============================================================================*/
// s2D class
/*----------------------------------------------------------------------------*/
    
    void s2D::to_Screen(const sString &indent) const
    {
	to_Stream(stdout, indent);
    }

    
    void s2D::to_Stream(FILE *fw, const sString &indent) const
    {
	fprintf(fw, "%s(%.3f,%.3f)\n", indent.c_str(), x, y);
    }


    
    
/*============================================================================*/
// s3D class
/*----------------------------------------------------------------------------*/
    
    void s3D::to_Screen(const sString &indent) const
    {
	to_Stream(stdout, indent);
    }

    
    void s3D::to_Stream(FILE *fw, const sString &indent) const
    {
	fprintf(fw, "%s(%.3f,%.3f,%.3f)\n", indent.c_str(), x, y, z);
    }


    


/*============================================================================*/
// sRobot class
/*----------------------------------------------------------------------------*/

    void s2DRobot::Arm::to_Screen(const sString &indent) const
    {
	to_Stream(stdout, indent);
    }

    
    void s2DRobot::Arm::to_Stream(FILE *fw, const sString &indent) const
    {
	fprintf(fw, "%sArm: (length = %.3f, rotation = %.3f)\n", indent.c_str(), length, rotation);
    }
    
    
/*----------------------------------------------------------------------------*/

    void s2DRobot::calc_EndPosition(const Arm &arm, const s2D &origin, s2D &end) const
    {
	end.x = origin.x + cos(arm.rotation) * arm.length;
	end.y = origin.y + sin(arm.rotation) * arm.length;
    }

    
/*----------------------------------------------------------------------------*/

} // namespace realX
