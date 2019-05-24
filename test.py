import itertools



num=2
lis=[]
for i in range(num):
	lis.append(True)
	lis.append(False)
lis=[]
for i in itertools.product([True,False],repeat=4):
	list_table_seq =list(i)
	list_table_seq.append(1.0)
	lis.append(list_table_seq)


for j in lis:
	res=j[0]
	for t in range(1,len(j)-2):
		res=res&j[t]
	j[-1] = (1.0 if (res==j[-2]) else 0.0)
print(lis )
