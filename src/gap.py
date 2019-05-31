import sys
import os

# cktlis=['17','432','499','880','1355','2670','3540','6288','7552','adder','bar','dec','max','multi','sin']

# cktlis=['17','432','499','880','1355','2670','3540','6288','7552']
	
cktlis = ['7552']
# for j in range(1000,100001,5000):
# 	for k in range(1,15):
for i in cktlis:
	os.system('bash sim.sh '+ i +' 1 ' + '8'+' '+'14000')
		
	# os.system('bash sim.sh 17 8 '+str(i))
	# os.system('bash sim.sh 432 8 '+str(i))
	# os.system('bash sim.sh 499 8 '+str(i))
	# os.system('bash sim.sh 880 8 '+str(i))
	# os.system('bash sim.sh 1355 8 '+str(i))
	# os.system('bash sim.sh 2670 8 '+str(i))
	# os.system('bash sim.sh 3540 8 '+str(i))
	# os.system('bash sim.sh 6288 8 '+str(i))
	# os.system('bash sim.sh 7552 8 '+str(i))