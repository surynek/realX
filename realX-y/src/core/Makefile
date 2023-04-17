all: debug

debug:	libCore_realX_dbg.a 

optimized:	libCore_realX_opt.a 

libCore_realX_dbg.a:	robot.o_dbg cnf.o_dbg network.o_dbg top_K.o_dbg 
	ar r libCore_realX_dbg.a robot.o_dbg cnf.o_dbg network.o_dbg top_K.o_dbg

robot.o_dbg:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h robot.h cnf.h network.h top_K.h robot.cpp
	g++ -Wall -Wextra -pedantic -Wno-class-memaccess -Wno-long-long -Wno-unused-result -Wno-sign-compare -Wno-delete-non-virtual-dtor -g -std=c++0x -c -I../../src -I../../src/core -orobot.o_dbg robot.cpp

cnf.o_dbg:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h robot.h cnf.h network.h top_K.h cnf.cpp
	g++ -Wall -Wextra -pedantic -Wno-class-memaccess -Wno-long-long -Wno-unused-result -Wno-sign-compare -Wno-delete-non-virtual-dtor -g -std=c++0x -c -I../../src -I../../src/core -ocnf.o_dbg cnf.cpp

network.o_dbg:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h robot.h cnf.h network.h top_K.h network.cpp
	g++ -Wall -Wextra -pedantic -Wno-class-memaccess -Wno-long-long -Wno-unused-result -Wno-sign-compare -Wno-delete-non-virtual-dtor -g -std=c++0x -c -I../../src -I../../src/core -onetwork.o_dbg network.cpp

top_K.o_dbg:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h robot.h cnf.h network.h top_K.h top_K.cpp
	g++ -Wall -Wextra -pedantic -Wno-class-memaccess -Wno-long-long -Wno-unused-result -Wno-sign-compare -Wno-delete-non-virtual-dtor -g -std=c++0x -c -I../../src -I../../src/core -otop_K.o_dbg top_K.cpp


libCore_realX_opt.a:	robot.o_opt cnf.o_opt network.o_opt top_K.o_opt 
	ar r libCore_realX_opt.a robot.o_opt cnf.o_opt network.o_opt top_K.o_opt

robot.o_opt:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h robot.h cnf.h network.h top_K.h robot.cpp
	g++ -std=c++0x -c -w -O9 -mtune=native -I../../src -I../../src/core -orobot.o_opt robot.cpp

cnf.o_opt:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h robot.h cnf.h network.h top_K.h cnf.cpp
	g++ -std=c++0x -c -w -O9 -mtune=native -I../../src -I../../src/core -ocnf.o_opt cnf.cpp

network.o_opt:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h robot.h cnf.h network.h top_K.h network.cpp
	g++ -std=c++0x -c -w -O9 -mtune=native -I../../src -I../../src/core -onetwork.o_opt network.cpp

top_K.o_opt:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h robot.h cnf.h network.h top_K.h top_K.cpp
	g++ -std=c++0x -c -w -O9 -mtune=native -I../../src -I../../src/core -otop_K.o_opt top_K.cpp


clean:
	rm -f -r *~ *.o_dbg *.o_opt *.a *.bak output*.txt
