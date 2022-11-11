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
/* basic_test.cpp / 0-107_nofutu                                              */
=======
/* basic_test.cpp / 0-104_nofutu                                              */
>>>>>>> 14f14b75c8f1d7815a0fb722e1fd0403f95fb68c
/*----------------------------------------------------------------------------*/
//
// Basic initial test.
//
/*----------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>

#include "defs.h"
#include "compile.h"
#include "result.h"
#include "version.h"

#include "common/types.h"
#include "core/robot.h"
#include "util/geometry.h"

#include "test/basic_test.h"


using namespace std;
using namespace realX;


/*----------------------------------------------------------------------------*/

namespace realX
{




/*----------------------------------------------------------------------------*/

void print_Introduction(void)
{
    printf("----------------------------------------------------------------\n");    
    printf("%s : Basic Test Program\n", sPRODUCT);
    printf("%s\n", sCOPYRIGHT);
    printf("================================================================\n");
}


int test_basic_1(void)
{
    printf("Testing basic 1 ...\n");
    s2DRobot robot_1;
    s2DRobot::Arm arm_1(2.0, 0.0);
    arm_1.to_Screen();

    s2D origin_1(0.0, 0.0);
    s2D end_1;

    robot_1.calc_EndPosition(arm_1, origin_1, end_1);
    end_1.to_Screen();
    
    printf("Testing basic 1 ... finished\n");
    for (sInt_32 d = 0; d <= 180; d += 10)
    {
	arm_1.rotation = sDEG_2_RAD(d);
	robot_1.calc_EndPosition(arm_1, origin_1, end_1);
	end_1.to_Screen();
    }

    return sRESULT_SUCCESS;
}


}  // namespace realX


/*----------------------------------------------------------------------------*/

int main(int sUNUSED(argc), const char **sUNUSED(argv))
{
    sResult result;

    print_Introduction();

    if (sFAILED(result = test_basic_1()))
    {
	printf("Test basic 1 failed (error:%d).\n", result);
	return result;
    }
    return 0;
}
