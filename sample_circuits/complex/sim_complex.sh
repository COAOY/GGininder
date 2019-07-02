echo "Crete all probablity files"

arr=("cadder.ckt" "cbar.ckt" "cdec.ckt" "cmax.ckt" "csin.ckt")

for ((i=0; i < ${#arr[@]}; i++))
do
    echo ${arr[$i]}
    python main.py sample_circuits/complex/${arr[i]}
       
done
    
