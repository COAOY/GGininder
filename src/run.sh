echo "atpg for c$1 with ndet = $2"
# time ./atpg -ndet $2 -anum 8 -tdfatpg -compression ../sample_circuits/c$1.ckt > log
time ./atpg -ndet $2 -anum 8  -cpdag -tdfatpg -compression ../sample_circuits/c$1.ckt > log
../bin/golden_tdfsim -ndet $2 -tdfsim patterns ../sample_circuits/c$1.ckt >> log
python3 util.py log
#cat ../tdf_patterns/c$1.pat
#../bin/golden_tdfsim -ndet $2 -tdfsim ../tdf_patterns/c$1.pat ../sample_circuits/c$1.ckt
# ./atpg -ndet $2 -tdfsim ../tdf_patterns/c$1.pat ../sample_circuits/c$1.ckt
