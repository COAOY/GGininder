

arr=("17" "880" "5315" "6288" "7552")
for ((i=0; i < ${#arr[@]}; i++))
do
	echo "[Level selection]"
# time ./atpg -ndet -anum 8 -tdfatpg -compression ../sample_circuits/c$1.ckt > ../logc$1
	time ./atpg -ssfatpg -ndet 1 ../sample_circuits/c${arr[$i]}.ckt > ../log1
	# ../bin/golden_tdfsim -ndet 1 -tdfsim ../log1 ../sample_circuits/bseries/b$1_C.bench.txt.ckt > ../log1
	python util.py ../log1 > cpdag_result/c${arr[$i]}_podem.txt

	echo "[CPDAG selection]"
	time ./atpg -ssfatpg -cpdag -ndet 1 ../sample_circuits/c${arr[$i]}.ckt > ../log2
	python util.py ../log2 > cpdag_result/c${arr[$i]}_bayesian.txt
done