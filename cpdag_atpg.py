import matplotlib
import matplotlib.pyplot as plt
import seaborn; seaborn.set_style('whitegrid')
import numpy

from copy import *
from pomegranate import *
from gate import *

# main class 
class CPD:
	"""
	Inilization parameter
	"""
	filename = None  	# the name of circuit file 
	bay= None 		# bayesian reference
	model = None
	wire = []
	Gate_dict = {}
	wire_dict = {}
	state_ref_dict = {}
	def __init__(self, filename):
		self.filename = filename
		self.model = BayesianNetwork("CPDAG")
		print("[Read File] Parsar")
		self.parsar()
		self.model.bake()

	def parsar(self):
		'''
		parasar file

		'''
		num=0
		infile=open(self.filename,'r')
		line_list=[]
		for line in infile:
			line_list.append(line)
		close(infile)

		for line in line_list:
			



		# for line in gate_list:
		# 	if(line[0]== 'i'):
		# 		inp=line.split()
		# 		w1 = Gate(deepcopy(line[0]),deepcopy(inp),self.model,deepcopy(num), self.Gate_dict )
		# 		self.wire.append(w1)
		# 		self.Gate_dict.setdefault(w1.return_name(),w1)
		# 		num=num+1
		# 		# print(inp[1])
		# 	elif(line[0]== 'o'):
		# 		continue
		# 		# inp=line.split()
		# 		# outwire.append(deepcopy(inp) )
		# 		# outnum.append(deepcopy(num))
		# 		# print(inp[1])
		# 	elif(line[0]== 'g'):
		# 		inp=line.split()
		# 		w1 = Gate(deepcopy(line[0]),deepcopy(inp[1:]),self.model,deepcopy(num),self.Gate_dict )
		# 		self.wire.append(w1)
		# 		print(w1.return_name())
		# 		self.Gate_dict.setdefault(w1.return_name(),w1)
		# 		num=num+1
		# 		# print(inp[5])
		# 	else:
		# 		# print(" input file have ilegal parameter")
		# 		pass
		# for k, v in self.Gate_dict.items():
		# 	v.build_conditional_prob()

	def logic_simulation(self):
		pass







	def return_bayRef(self):
		return self.model
#





