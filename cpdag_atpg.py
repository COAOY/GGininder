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
		only implement nand,and,or gate and input
		'''
		num=0
		infile=open(self.filename,'r')
		outwire = []
		outnum = []
		for line in infile:
			if(line[0]== 'i'):
				inp=line.split()
				w1 = Gate(deepcopy(line[0]),deepcopy(line),self.model,deepcopy(inp[1]),deepcopy(num), self.wire_dict, self.state_ref_dict )
				self.wire.append(w1)
				# print(inp[1])
			elif(line[0]== 'o'):
				inp=line.split()
				outwire.append(deepcopy(inp) )
				outnum.append(deepcopy(num))
				# print(inp[1])
			elif(line[0]== 'g'):
				inp=line.split()
				w1 = Gate(deepcopy(line[0]),deepcopy(inp[1:]),self.model,deepcopy(inp[5]),deepcopy(num), self.wire_dict, self.state_ref_dict )
				self.wire.append(w1)
				# print(inp[5])
			else:
				pass
			num=num+1
		
	def return_bayRef(self):
		return self.model
#


