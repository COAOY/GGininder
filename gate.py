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
	def __init__(self, arrtibute, sequence, bay_ref, gid, gate_dic):
		self.arrt = arrtibute
		self.seq = sequence
		self.bay_ref = bay_ref
		self.gid = gid
		self.gate_dic=gate_dic
		if self.arrt == 'i':
			# arrtibute: input
			self.name = self.seq[1]
			self.add_Input_to_bayesian()
		elif self.arrt == 'g':
			# arrtibute: gate
			if self.seq[0]=='nand':
				if len(self.seq)==5:
					self.name = self.seq[4]
				elif len(self.seq)==4:
					self.name = self.seq[3]
				else:
					print(" input file have ilegal NAND gate")
			elif self.seq[0]=='and':
				if len(self.seq)==5:
					self.name = self.seq[4]
				elif len(self.seq)==4:
					self.name = self.seq[3]
				else:
					print(" input file have ilegal AND gate")
			elif self.seq[0]=='or':
				if len(self.seq)==5:
					self.name = self.seq[4]
				elif len(self.seq)==4:
					self.name = self.seq[3]
				else:
					print(" input file have ilegal OR gate")
			elif self.seq[0]=='not':
				self.name = self.seq[3]
			else:
				print(" ilegal gate!!")
		
		
	def build_conditional_prob(self):
		if self.arrt == 'i':
			# arrtibute: input
			self.name = self.seq[1]
			self.add_Input_to_bayesian()
		elif self.arrt == 'g':
			# arrtibute: gate
			if self.seq[0]=='nand':
				if len(self.seq)==5:
					self.name = self.seq[4]
					self.add_Nand_to_bayesian(self.seq[1],self.seq[2])
				elif len(self.seq)==4:
					self.name = self.seq[3]
					self.add_Nand_to_bayesian(self.seq[1],self.seq[1])
				else:
					print(" input file have ilegal NAND gate")
			elif self.seq[0]=='and':
				if len(self.seq)==5:
					self.name = self.seq[4]
					self.add_And_to_bayesian(self.seq[1],self.seq[2])
				elif len(self.seq)==4:
					self.name = self.seq[3]
					self.add_And_to_bayesian(self.seq[1],self.seq[1])
				else:
					print(" input file have ilegal AND gate")
			elif self.seq[0]=='or':
				if len(self.seq)==5:
					self.name = self.seq[4]
					self.add_Or_to_bayesian(self.seq[1],self.seq[2])
				elif len(self.seq)==4:
					self.name = self.seq[3]
					self.add_Or_to_bayesian(self.seq[1],self.seq[1])
				else:
					print(" input file have ilegal OR gate")
			elif self.seq[0]=='not':
				self.name = self.seq[3]
				self.add_Not_to_bayesian(self.seq[1])
			else:
				print(" ilegal gate!!")

	def return_bayref(self):
		return self.bay_ref
	
	def add_Input_to_bayesian(self):
		self.gcref = DiscreteDistribution({True: 0.5, False: 0.5})
		self.gsref = State(self.gcref, name=self.name)
		self.bay_ref.add_states(self.gsref)
	

	def add_Nand_to_bayesian(self, d1, d2):

		self.gcref =ConditionalProbabilityTable(
        [[True,  True,  True,  0.0],
         [True,  True,  False, 1.0],
         [True,  False, True,  1.0],
         [True,  False, False, 0.0],
         [False, True,  True,  1.0],
         [False, True,  False, 0.0],
         [False, False, True,  1.0],
         [False, False, False, 0.0]], [self.gate_dic[d1].return_gcref(), self.gate_dic[d2].return_gcref()])
		self.gsref = State(self.gcref, name=self.name)
		self.bay_ref.add_states(self.gsref)
		self.bay_ref.add_edge(self.gate_dic[d1].return_gsref(),self.gsref)
		self.bay_ref.add_edge(self.gate_dic[d2].return_gsref(),self.gsref)
		


	def add_And_to_bayesian(self, d1, d2):

		self.gcref =ConditionalProbabilityTable(
        [[True,  True,  True,  1.0],
         [True,  True,  False, 0.0],
         [True,  False, True,  0.0],
         [True,  False, False, 1.0],
         [False, True,  True,  1.0],
         [False, True,  False, 0.0],
         [False, False, True,  0.0],
         [False, False, False, 1.0]], [self.gate_dic[d1].return_gcref(), self.gate_dic[d2].return_gcref()])
		self.gsref = State(self.gcref, name=self.name)
		self.bay_ref.add_states(self.gsref)
		self.bay_ref.add_edge(self.gate_dic[d1].return_gsref(),self.gsref)
		self.bay_ref.add_edge(self.gate_dic[d2].return_gsref(),self.gsref)
		



	def add_Or_to_bayesian(self, d1, d2):

		self.gcref =ConditionalProbabilityTable(
        [[True,  True,  True,  1.0],
         [True,  True,  False, 0.0],
         [True,  False, True,  1.0],
         [True,  False, False, 0.0],
         [False, True,  True,  1.0],
         [False, True,  False, 0.0],
         [False, False, True,  0.0],
         [False, False, False, 1.0]], [self.gate_dic[d1].return_gcref(), self.gate_dic[d2].return_gcref()])
		self.gsref = State(self.gcref, name=self.name)
		self.bay_ref.add_states(self.gsref)
		self.bay_ref.add_edge(self.gate_dic[d1].return_gsref(),self.gsref)
		self.bay_ref.add_edge(self.gate_dic[d2].return_gsref(),self.gsref)
		



	def add_Not_to_bayesian(self, d1):

		self.gcref =ConditionalProbabilityTable(
        [[True,  True,  0.0],
         [True,  False, 1.0],
         [False, True, 1.0],
         [False,  False, 0.0],], [self.gate_dic[d1].return_gcref()])
		self.gsref = State(self.gcref, name=self.name)
		self.bay_ref.add_states(self.gsref)
		self.bay_ref.add_edge(self.gate_dic[d1].return_gsref(),self.gsref)
		


	def return_name(self):
		return self.name
	def return_gcref(self):
		return self.gcref
	def return_gsref(self):
		return self.gsref