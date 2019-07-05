# echo "atpg for c$1 with ndet ="
echo "[Level selection]"
# time ./atpg -ndet -anum 8 -tdfatpg -compression ../sample_circuits/c$1.ckt > ../logc$1
time ./atpg -ssfatpg -ndet 1 ../sample_circuits/bseries/b$1_C.bench.txt.ckt > ../log1
# ../bin/golden_tdfsim -ndet 1 -tdfsim ../log1 ../sample_circuits/bseries/b$1_C.bench.txt.ckt > ../log1
python util.py ../log1

echo "[CPDAG selection]"
time ./atpg -ssfatpg -bayesian -ndet 1 ../sample_circuits/bseries/b$1_C.bench.txt.ckt > ../log2
# ../bin/golden_tdfsim -ndet 1 -tdfsim ../log2 ../sample_circuits/bseries/b$1_C.bench.txt.ckt > ../log2
python util.py ../log2

#cat ../tdf_patterns/c$1.pat
#../bin/golden_tdfsim -ndet $2 -tdfsim ../tdf_patterns/c$1.pat ../sample_circuits/c$1.ckt
# ./atpg -ndet $2 -tdfsim ../tdf_patterns/c$1.pat ../sample_circuits/c$1.ckt

# ./atpg -tdfatpg -ndet 1 ~/TestingFinal_2018/sample_circuits/c7552.ckt > c17.pat
# ~/TestingFinal_2018/bin/golden_tdfsim -ndet 1 -tdfsim c17.pat ~/TestingFinal_2018/sample_circuits/c7552.ckt