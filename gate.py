import matplotlib
import matplotlib.pyplot as plt
import seaborn; seaborn.set_style('whitegrid')
import numpy
import copy
from pomegranate import *


class Gate:

	arrt = None      
	seq = None
	bay_ref = None
	name = None
	gid = None
	gcref = None
	gsref = None
	'''
	arrtibute(string): the type of gate, input(i), ouput(o), gate(g)
	sequence(string) : read line from file
	bay_ref(ref)	 : baysian reference
	name(string): the name of gate
	gid(int) : gate id in the bay list
	gcref(ref) = gate condition refference
	gsref(ref) = gate state refference
	'''
	def __init__(self, arrtibute, sequence, bay_ref, name, gid, bay_dic, state_ref_dic):
		self.arrt = arrtibute
		self.seq = sequence
		self.bay_ref = bay_ref
		self.name = name
		self.gid = gid
		self.bay_dic = bay_dic
		self.state_ref_dic = state_ref_dic 
		if self.arrt == 'i':
			self.add_Input_to_bayesian()
		elif self.arrt == 'g':
			if self.seq[0]=='nand':
				# print(self.seq[1],self.seq[2])
				self.add_Nand_to_bayesian(self.seq[1],self.seq[2])

	def return_bayref(self):
		return self.bay_ref
	
	def add_Input_to_bayesian(self):
		self.gcref = DiscreteDistribution({True: 0.5, False: 0.5})
		self.gsref = State(self.gcref, name=self.name)
		self.bay_ref.add_states(self.gsref)
		
		self.bay_dic.setdefault(self.name,self.gcref)
		self.state_ref_dic.setdefault(self.name, self.gsref)

	def add_Nand_to_bayesian(self, d1, d2):

		self.gcref =ConditionalProbabilityTable(
        [[True,  True,  True,  1.0],
         [True,  True,  False, 0.0],
         [True,  False, True,  0.0],
         [True,  False, False, 1.0],
         [False, True,  True,  1.0],
         [False, True,  False, 0.0],
         [False, False, True,  1.0],
         [False, False, False, 0.0]], [self.bay_dic[d1], self.bay_dic[d2]])
		self.gsref = State(self.gcref, name=self.name)
		self.bay_ref.add_states(self.gsref)
		self.bay_ref.add_edge(self.state_ref_dic[d1],self.gsref)
		self.bay_ref.add_edge(self.state_ref_dic[d2],self.gsref)
		
		self.bay_dic.setdefault(self.name, self.gcref)
		self.state_ref_dic.setdefault(self.name, self.gsref)








