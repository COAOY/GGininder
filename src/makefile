#

CFLAGS = -std=c++11 -I../../dlib-19.17 
LFLAG = -L/usr/X11/lib
SOURCE = ../../dlib-19.17/dlib/all/source.cpp
FILES = source.o atpg.o bayesian.o tpgmain.o input.o level.o sim.o podem.o test.o init_flist.o faultsim.o tdfsim.o display.o tdfatpg.o stc.o dtc.o
# export LDFLAGS="-L/usr/local/opt/openblas/lib"
# export CPPFLAGS="-I/usr/local/opt/openblas/include"
# export PKG_CONFIG_PATH="/usr/local/opt/openblas/lib/pkgconfig"
# ln -s /opt/X11 X11
atpg : $(FILES) 
	g++ $(CFLAGS) $(FILES) $(LFLAG) -lpthread -lX11 -lm -o atpg
source.o: $(SOURCE)
	g++ $(CFLAGS) $(LFLAG) -c $(SOURCE)
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
bayesian.o : atpg.h bayesian.cpp
	g++ $(CFLAGS) -c bayesian.cpp
clean:
	rm `ls *.o | egrep -v source.o`
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
