echo "atpg for c$1 with ndet = 1"
echo "[Traditional selection]"
time ./atpg -tdfatpg ../sample_circuits/c$1.ckt > log1
# ../bin/golden_tdfsim -tdfsim patterns ../sample_circuits/c$1.ckt >> log1
python3 util.py log1
echo ""
echo "[CPDAG Mode]"
time ./atpg -tdfatpg -cpdag ../sample_circuits/c$1.ckt > log2
# ../bin/golden_tdfsim -tdfsim patterns ../sample_circuits/c$1.ckt >> log2
python3 util.py log2
#cat ../tdf_patterns/c$1.pat
#../bin/golden_tdfsim -ndet $2 -tdfsim ../tdf_patterns/c$1.pat ../sample_circuits/c$1.ckt
# ./atpg -ndet $2 -tdfsim ../tdf_patterns/c$1.pat ../sample_circuits/c$1.ckt
