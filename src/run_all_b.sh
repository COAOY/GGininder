

arr=("15" "20" "21" "22")
for ((i=0; i < ${#arr[@]}; i++))
do
    echo ${arr[$i]}
    # python main.py sample_circuits/complex/${arr[i]}
    echo "[Level selection]"
	time ./atpg -ssfatpg -ndet 1 ../sample_circuits/bseries/b${arr[$i]}_C.bench.txt.ckt > ../log1
# <<<<<<< HEAD
# 	python util.py ../log1 > cpdag_result/b${arr[$i]}_podem.txt

# 	echo "[CPDAG selection]"
# 	time ./atpg -ssfatpg -cpdag -ndet 1 ../sample_circuits/bseries/b${arr[$i]}_C.bench.txt.ckt > ../log2
# 	python util.py ../log2 > cpdag_result/b${arr[$i]}_bayesian.txt
# =======
	python util.py ../log1

	echo "[CPDAG selection]"
	time ./atpg -ssfatpg -cpdag -ndet 1 ../sample_circuits/bseries/b${arr[$i]}_C.bench.txt.ckt > ../log2
	python util.py ../log2
# >>>>>>> c06311ae6238db99f012e9234e2b14b68f819ecd
       
done
    