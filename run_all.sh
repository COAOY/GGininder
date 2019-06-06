arr=("432" "499" "880" "1355" "5315" "6288" "7552")
 
for ((i=0; i < ${#arr[@]}; i++))
do
    echo ${arr[$i]}
    python main.py sample_circuits/c${arr[$i]}.ckt
done