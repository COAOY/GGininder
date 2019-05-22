import matplotlib
import matplotlib.pyplot as plt
import seaborn; seaborn.set_style('whitegrid')
import numpy

from pomegranate import *
from gate import *

# main class 
class CPD:
	"""
	Inilization parameter
	"""
	filename = None  	# the name of circuit file 
	bay= None 		# bayesian reference

	def __init__(self, filename):
		self.filename = filename
		self.model = BayesianNetwork("CPDAG")
		print("[Read File] Parsar")
		self.parsar()
		

	def parsar(self):
		num=0
		infile=open(self.filename,'r')
		for line in infile:
			if(line[0]== 'i'):
				inp=line.split()
				print(inp[1])
			elif(line[0]== 'o'):
				inp=line.split()
				print(inp[1])
			elif(line[0]== 'g'):
				inp=line.split()
				print(inp[1:])
			else:
				pass
		

#


