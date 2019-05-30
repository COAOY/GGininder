#

CFLAGS = -std=c++11 -O3 

FILES = atpg.o tpgmain.o input.o level.o sim.o podem.o test.o init_flist.o faultsim.o tdfsim.o display.o tdfatpg.o stc.o dtc.o

atpg : $(FILES) 
	g++ $(CFLAGS) $(FILES) -lm -o atpg

atpg.o : atpg.h atpg.cpp
	g++ $(CFLAGS) -c atpg.cpp
tpgmain.o : atpg.h tpgmain.cpp
	g++ $(CFLAGS) -c tpgmain.cpp
input.o : atpg.h input.cpp
	g++ $(CFLAGS) -c input.cpp
level.o : atpg.h level.cpp
	g++ $(CFLAGS) -c level.cpp
sim.o : atpg.h logic_tbl.h sim.cpp
	g++ $(CFLAGS) -c sim.cpp
podem.o : atpg.h podem.cpp
	g++ $(CFLAGS) -c podem.cpp
init_flist.o : atpg.h init_flist.cpp
	g++ $(CFLAGS) -c init_flist.cpp
faultsim.o : atpg.h faultsim.cpp
	g++ $(CFLAGS) -c faultsim.cpp
tdfsim.o : atpg.h tdfsim.cpp
	g++ $(CFLAGS) -c tdfsim.cpp
test.o : atpg.h test.cpp
	g++ $(CFLAGS) -c test.cpp
display.o : atpg.h display.cpp
	g++ $(CFLAGS) -c display.cpp
tdfatpg.o : atpg.h tdfatpg.cpp
	g++ $(CFLAGS) -c tdfatpg.cpp
stc.o : atpg.h stc.cpp
	g++ $(CFLAGS) -c stc.cpp
dtc.o : atpg.h dtc.cpp
	g++ $(CFLAGS) -c dtc.cpp
clean:
	rm *.o
c:
	# rm tpgmain.o
	# g++ $(CFLAGS) -c tpgmain.cpp
	rm atpg
	rm tdfatpg.o
	rm podem.o
	g++ $(CFLAGS) -c tdfatpg.cpp
	g++ $(CFLAGS) -c podem.cpp
	g++ $(CFLAGS) $(FILES) -lm -o atpg
	# ./atpg -tdfatpg ../sample_circuits/c17.ckt > ../tdf_patterns/c17.pat
	./atpg -tdfatpg ../sample_circuits/c17.ckt # > ../tdf_patterns/c17.pat
	# ./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c17.ckt  
teststc:
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c17.ckt  
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c432.ckt
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c499.ckt
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c880.ckt
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c1355.ckt
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c1908.ckt
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c2670.ckt
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c3540.ckt
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c5315.ckt
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c6288.ckt
	./atpg -tdfatpg -ndet 1 -compression ../sample_circuits/c7552.ckt
direct:
	g++ $(CFLAGS)$(FILES:.o=.cpp) -o atpg
