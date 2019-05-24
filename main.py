import sys
from cpdag_atpg import *


















def main():
	if(len(sys.argv) == 2):
		argv = sys.argv
	# try:
		print("[main] program start")
		cpd = CPD(argv[1])
		cpd.return_bayRef().plot("Circuit")
		# print(cpd.return_bayRef().predict_proba({"1GAT(0)":True}))

		print("[main] program end")
	# except:
	# 	print("[Wrong!!] ")

if __name__ == '__main__':
    main()
    
