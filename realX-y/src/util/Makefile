all: debug

debug:	libUtil_realX_dbg.a 

optimized:	libUtil_realX_opt.a 

libUtil_realX_dbg.a:	io.o_dbg geometry.o_dbg statistics.o_dbg 
	ar r libUtil_realX_dbg.a io.o_dbg geometry.o_dbg statistics.o_dbg

io.o_dbg:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h io.h geometry.h statistics.h io.cpp
	g++ -Wall -Wextra -pedantic -Wno-class-memaccess -Wno-long-long -Wno-unused-result -Wno-sign-compare -Wno-delete-non-virtual-dtor -g -std=c++0x -c -I../../src -I../../src/util -oio.o_dbg io.cpp

geometry.o_dbg:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h io.h geometry.h statistics.h geometry.cpp
	g++ -Wall -Wextra -pedantic -Wno-class-memaccess -Wno-long-long -Wno-unused-result -Wno-sign-compare -Wno-delete-non-virtual-dtor -g -std=c++0x -c -I../../src -I../../src/util -ogeometry.o_dbg geometry.cpp

statistics.o_dbg:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h io.h geometry.h statistics.h statistics.cpp
	g++ -Wall -Wextra -pedantic -Wno-class-memaccess -Wno-long-long -Wno-unused-result -Wno-sign-compare -Wno-delete-non-virtual-dtor -g -std=c++0x -c -I../../src -I../../src/util -ostatistics.o_dbg statistics.cpp


libUtil_realX_opt.a:	io.o_opt geometry.o_opt statistics.o_opt 
	ar r libUtil_realX_opt.a io.o_opt geometry.o_opt statistics.o_opt

io.o_opt:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h io.h geometry.h statistics.h io.cpp
	g++ -std=c++0x -c -w -O9 -mtune=native -I../../src -I../../src/util -oio.o_opt io.cpp

geometry.o_opt:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h io.h geometry.h statistics.h geometry.cpp
	g++ -std=c++0x -c -w -O9 -mtune=native -I../../src -I../../src/util -ogeometry.o_opt geometry.cpp

statistics.o_opt:	../../src/version.h ../../src/defs.h ../../src/config.h ../../src/compile.h ../../src/result.h io.h geometry.h statistics.h statistics.cpp
	g++ -std=c++0x -c -w -O9 -mtune=native -I../../src -I../../src/util -ostatistics.o_opt statistics.cpp


clean:
	rm -f -r *~ *.o_dbg *.o_opt *.a *.bak output*.txt
