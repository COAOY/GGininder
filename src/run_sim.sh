echo "golden_atpg for c$1"
../bin/golden_tdfsim -ndet $2 -tdfsim ../golden_patterns/c$1.pat ../sample_circuits/c$1.ckt

echo ""
echo ""
echo "self_atpg for c$1"
./atpg -ndet $2 -tdfsim ../golden_patterns/c$1.pat ../sample_circuits/c$1.ckt
