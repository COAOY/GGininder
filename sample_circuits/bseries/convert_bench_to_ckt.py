import sys
import re

argv = sys.argv
file = open(argv[1],'r')
ofile = []
p1 = re.compile(r'[(](.*?)[)]', re.S)
total_num=1
for line in file:
	if line[0] != '#' and line!='\n':
		q = line.split()
		if len(q) == 1:
			# input or ouput
			t=line[:-2]
			t=t.split('(')
			if t[0]=='INPUT':
				ofile.append('i '+t[1]+'\n')
			else:
				ofile.append('o '+t[1]+'\n')
		else:
			t=line.split('(')[0].split()
			s = re.findall(p1, line)
			s = s[0].replace(', ',' ')
			t[2]=t[2].lower()
			ofile.append('g'+str(total_num) +' '+  t[2]  + ' '+ s +' ; '+ t[0] + '\n' )
			total_num+=1

file.close()
ofi = open( argv[1]+'.ckt','w')
for i in ofile:
	ofi.write(i)
ofi.close()