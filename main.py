import sys
import time
from cpdag_atpg import *













def main():
	if(len(sys.argv) == 2):
		argv = sys.argv
	# try:
		print("[main] Bayesain ATPG  =3=")
		cpd = CPD(argv[1])
		# cpd.return_bayRef().plot("Circuit")
		cpd.logic_simulation()
		print('[Operation]\n ..Probability operation')

		start = time.time()

		nlist = (cpd.return_bayRef().predict_proba({}))
		# for i in range(0,len(nlist)):
			# print(nlist[i]['parameters'])
		end = time.time()
		print(' ..predict_proba run time:',(end-start),'seconds' )
		# cpd.return_bayRef().predict({'545GAT(287)':True})
		# print(len(cpd.return_bayRef().predict_proba({'22GAT(10)':True})))
		# print(cpd.return_bayRef().predict_proba({}))
		# cpd.return_bayRef().plot(argv[2])
		# print(cpd.return_bayRef().predict_proba({"1GAT(0)":True}))

		print(" ..Write file")
		cpd.write_iniline_probability_table( str(argv[1]+'.out'))
		print("[main] program end")
	# except:
	# 	print("[Wrong!!] ")

if __name__ == '__main__':
    main()
    
