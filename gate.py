import matplotlib
import matplotlib.pyplot as plt
import seaborn; seaborn.set_style('whitegrid')
import numpy
import copy
import itertools
from pomegranate import *


class Gate:

	arrt = None      
	seq = None
	bay_ref = None
	name = None
	gid = None
	gtype = None
	gcref = None
	gsref = None
	rely_list = []
	'''
	arrtibute(string): the type of gate, input(i), gate(g)
	sequence(string) : read line from file
	bay_ref(ref)	 : baysian reference
	name(string): the name of gate
	gid(int) : gate id in the bay list
	gtype(string): type of gate ex nand, and, or ~
	gcref(ref): gate condition refference
	gsref(ref): gate state refference
	rely_list[list]: the gate input in this gate 
	'''
	def __init__(self, arrtibute, sequence, bay_ref, gid, gate_dic):
		self.arrt = arrtibute
		self.seq = sequence
		self.bay_ref = bay_ref
		self.gid = gid
		self.gate_dic = gate_dic
		#set gtype
		# set input
		if(self.arrt=='g'):
			self.gtype = self.seq[1]
			for i in range(2,len(self.seq)):
				self.rely_list.append(self.seq[i])

		# set name
		for i in range(len(self.seq)):
			if self.seq[i] == ';':
				self.name = self.seq[i+1]
				break
		
		
		

	def bay_conditional_prob_buid(self):
		'''
		plz use this fun to create abyesian
		'''
		if(self.arrt=='i'):
			self.add_Input_to_bayesian()
		else:
			self.build_gate_conditional_prob()

	def build_gate_conditional_prob(self):
		'''
			buid baye condition except input
			plz do not use input in this fun.
		'''
		number_of_input = self.rely_list
		table = self.assign_ouput_to_true_tble(self.build_true_table(number_of_input),self.gtype)
		# find others gcref ref and create list
		glist=[]
		for i in self.rely_list:
			glist.append(self.gate_dic[i].return_gcref())

		self.gcref =ConditionalProbabilityTable(table,glist)
		self.gsref = State(self.gcref, name=self.name)
		self.bay_ref.add_states(self.gsref)
		#create edge
		for i in len(glist):
			self.bay_ref.add_edge(i,self.gsref)


	def return_bayref(self):
		return self.bay_ref
	
	def add_Input_to_bayesian(self):
		self.gcref = DiscreteDistribution({True: 0.5, False: 0.5})
		self.gsref = State(self.gcref, name=self.name)
		self.bay_ref.add_states(self.gsref)
		
	def build_true_table(self, input_number):
		'''
		input: 
		[int] input_number: # of gate port 
		return:
		[list] true table 
		'''
		lis=[]
		for i in itertools.product([True,False],repeat=input_number+1):
			list_table_seq =list(i)
			list_table_seq.append(0.0)
			lis.append(list_table_seq)

		return lis

	def assign_ouput_to_true_tble(self, table, gtype):
		'''
		!!!!! can not process input wire !!!!!
		input
		[list]table: true table
		[string] gtype: gate type
		return 
		[list]table: re assign gate output
		'''
		for j in table:
			res=j[0]
			for t in range(1,len(j)-2):
				if(gtype=='and'):
					res = res&j[t]
				elif(gtype=='nand'):
					res = res&j[t]
				elif(gtype=='or'):
					res = res|j[t]
				elif(gtype=='xor'):
					res = res^j[t]
				elif(gtype=='not'):
					res = not res
				else:
					print('wronf gtype in gate.py')
			if(gtype=='nand'):
				res = not res
			if(gtype=='xor' and len(j)==3):
				res = res^res
			j[-1] = (1.0 if (res==j[-2]) else 0.0)
		return table


	def return_name(self):
		return self.name
	def return_gcref(self):
		return self.gcref
	def return_gsref(self):
		return self.gsref









