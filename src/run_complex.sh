echo "Crete all probablity files"

arr=("cadder.ckt" "cbar.ckt" "cdec.ckt" "cmax.ckt" "csin.ckt" "cmulti.ckt")

for ((i=0; i < ${#arr[@]}; i++))
do
    echo ${arr[$i]}
    # python main.py sample_circuits/complex/${arr[i]}
    echo "[Level selection]"
	time ./atpg -tdfatpg -ndet 1 ../sample_circuits/complex/${arr[i]} > ../log1
	python util.py ../log1

	echo "[CPDAG selection]"
	time ./atpg -tdfatpg -cpdag -ndet 1 ../sample_circuits/complex/${arr[i]} > ../log2
	python util.py ../log2
       
done
    