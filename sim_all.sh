echo "Run Simulation"

arr=("18" "19")
# arr=("test")

for ((i=0; i < ${#arr[@]}; i++))
do
	filename=log${arr[$i]}.txt
    touch $filename
    echo ${arr[$i]}
    # python main.py  $(pwd)/sample_circuits/bseries/b${arr[i]}_C.bench.txt.ckt # > $filename

    echo " " # >> $filename
    echo " " # >> $filename
    echo "[Level selection]" #>> $filename
	time $(pwd)/src/./atpg -tdfatpg -ndet 1 $(pwd)/sample_circuits/bseries/b${arr[i]}_C.bench.txt.ckt > log1_${arr[$i]}
	python $(pwd)/src/util.py log1_${arr[$i]} #>> $filename

	echo " " # >> $filename
    echo " " #>> $filename
	echo "[CPDAG selection]" #>> $filename
	time $(pwd)/src/./atpg -tdfatpg -cpdag -ndet 1 $(pwd)/sample_circuits/bseries/b${arr[i]}_C.bench.txt.ckt > log${arr[$i]}
	python $(pwd)/src/util.py log${arr[$i]} #>> $filename
   

       
done
    
