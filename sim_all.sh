echo "Crete all probablity files"

arr=("c17.ckt" "c432.ckt" "c499.ckt" "c880.ckt" "c1355.ckt" "c2670.ckt" "c5315.ckt" "c6288.ckt" "c7552.ckt")

for ((i=0; i < ${#arr[@]}; i++))
do
    echo ${arr[$i]}
    python main.py sample_circuits/${arr[i]}
       
done
    
