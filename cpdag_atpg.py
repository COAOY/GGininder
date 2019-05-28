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
	Gate_dict = {}
	input_wire_set = set()
	output_wire_set = set()
	output_wire = []
	gmt=[]
	exe_set = set()


	def __init__(self, filename):
		self.filename = filename
		self.model = BayesianNetwork("CPDAG")
		print("[Read File] Parsar")
		self.parsar()
		

	def parsar(self):
		'''
		parasar file
		because the 
		condatitional probability come from the gate 
		our wire mean the ouput of gate 
		'''
		num=0
		infile=open(self.filename,'r')
		line_list=[]
		for line in infile:
			inp = line.strip()
			inp = line.split()

			# inilization wire name
			if line[0]=='i' :
				w1 = Gate(deepcopy(inp[0]),deepcopy(inp[1:]),self.model,deepcopy(num), self.Gate_dict, self.exe_set, self.gmt)
				self.Gate_dict.setdefault(w1.return_name(),w1)
				self.input_wire_set.add(inp[1])
				num+=1
			elif line[0]=='g' :
				w1 = Gate(deepcopy(inp[0]),deepcopy(inp[1:]),self.model,deepcopy(num), self.Gate_dict, self.exe_set, self.gmt )
				self.Gate_dict.setdefault(w1.return_name(),w1)
				num+=1
			elif line[0]=='o' :
				self.output_wire_set.add(inp[1])
				self.output_wire.append(inp[1])
			
		infile.close()




	def logic_simulation(self):
		# print(self.Gate_dict)
		# set input wire
		print('[parsar]\n ..Logic simulation' )
		for i in self.input_wire_set:
			self.Gate_dict[i].bay_conditional_prob_buid()
		'''
		set wire form bottom to top
		to satisfy the mechanism of pomegrante
		'''
		for i in self.Gate_dict:
			if(i in self.exe_set):
				pass
			else:
				self.Gate_dict[i].bay_conditional_prob_buid()

		self.model.bake()
		print(' ..Number of gates   :', len(self.Gate_dict))
		print(' ..Number of inputs  :', len(self.input_wire_set))
		print(' ..Number of outputs :', len(self.output_wire_set))


		# print(self.input_wire_set)
		# print(self.gmt)
		# print('')
		# self.model.plot(self.filename)
		# 
		# print(self.model.predict_proba({self.output_wire[1]:True}))

	def write_iniline_probability_table(self, filename='output.txt'):
		ofile = open(filename, 'w')
		nlist = (self.model.predict_proba({}))
		for i in range(0,len(self.gmt)):
			ofile.write(self.gmt[i]+'\n')
			
			t=(' '.join((str(nlist[i]).split(':'))) )
			t=(' '.join((t.split(','))) )
			words= t.split()
			for i in range(0,len(words)):
				if words[i]=='"True"':
					ofile.write(('True '+ str(round(float(words[i+1]),4))))
					break	
			for i in range(0,len(words)):
				if words[i]=='"False"':
					ofile.write(('\nFalse '+ str(round(float(words[i+1]),4))+'\n\n'))
					break
			# ofile.write(str(nlist[i])+'\n' )
		ofile.close()



	def return_bayRef(self):
		return self.model
#





