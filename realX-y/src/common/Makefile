all: debug

debug:	libCommon_realX_dbg.a 

optimized:	libCommon_realX_opt.a 

libCommon_realX_dbg.a:	types.o_dbg graph.o_dbg 
	ar r libCommon_realX_dbg.a types.o_dbg graph.o_dbg

types.o_dbg:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h types.h graph.h types.cpp
	g++ -Wall -Wextra -pedantic -Wno-class-memaccess -Wno-long-long -Wno-unused-result -Wno-sign-compare -Wno-delete-non-virtual-dtor -g -std=c++0x -c -I../../src -I../../src/common -otypes.o_dbg types.cpp

graph.o_dbg:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h types.h graph.h graph.cpp
	g++ -Wall -Wextra -pedantic -Wno-class-memaccess -Wno-long-long -Wno-unused-result -Wno-sign-compare -Wno-delete-non-virtual-dtor -g -std=c++0x -c -I../../src -I../../src/common -ograph.o_dbg graph.cpp


libCommon_realX_opt.a:	types.o_opt graph.o_opt 
	ar r libCommon_realX_opt.a types.o_opt graph.o_opt

types.o_opt:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h types.h graph.h types.cpp
	g++ -std=c++0x -c -w -O9 -mtune=native -I../../src -I../../src/common -otypes.o_opt types.cpp

graph.o_opt:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h types.h graph.h graph.cpp
	g++ -std=c++0x -c -w -O9 -mtune=native -I../../src -I../../src/common -ograph.o_opt graph.cpp


clean:
	rm -f -r *~ *.o_dbg *.o_opt *.a *.bak output*.txt
