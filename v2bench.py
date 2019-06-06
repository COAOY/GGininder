import sys

supported_gate = ['and', 'nand', 'or', 'nor', 'xor', 'xnor', 'buf', 'not']

class Design: 
    def __init__(self, lines): 
        self.pis = []
        self.pos = []
        self.combs = []
        
        self.ppis = []
        self.ppos = []

        self.__parse(lines)

    def write(self, fname): 
        fo = open(fname, 'w')
        for i in self.pis+self.ppis: 
            fo.write('INPUT('+i+')\n')

        fo.write('\n')
        for o in self.pos+self.ppos: 
            fo.write('OUTPUT('+o+')\n')

        fo.write('\n')
        for c in self.combs: 
            fo.write(c[0]+' = '+c[1].upper()+'('+', '.join(c[2])+')\n')

        fo.close() 

    def __parse(self, lines): 
        pos = 0 
        while pos<len(lines): 
            line = lines[pos].strip()
            if line=='':
                pos += 1 
                continue
            keyword = line.split()[0].strip()
            if keyword=='input': # TODO: remove CK, VDD
                input_started = False 
                input_ready = False 
                while pos<len(lines): 
                    input_line = lines[pos].strip() 
                    if input_line=='':
                        continue
                    if not input_started: 
                        input_line = input_line.split()[1]
                        input_started = True 
                    self.pis+=[i for i in input_line.split(',') if i]
                    if input_line[-1]==';': 
                        input_ready = True
                        self.pis[-1] = self.pis[-1][:-1]
                        break
                    pos += 1 
                assert input_ready, 'Unexpected EOF.'
            elif keyword=='output': 
                output_started = False 
                output_ready = False 
                while pos<len(lines): 
                    output_line = lines[pos].strip() 
                    if output_line=='':
                        continue
                    if not output_started: 
                        output_line = output_line.split()[1]
                        output_started = True 
                    self.pos+=[o for o in output_line.split(',') if o]
                    if output_line[-1]==';': 
                        output_ready = True
                        self.pos[-1] = self.pos[-1][:-1]
                        break
                    pos += 1 
                assert output_ready, 'Unexpected EOF.'
            elif keyword in supported_gate: 
                assert line[-1]==';', 'Unexpected line continuation. '
                comb_line = line[line.find('(')+1:-2].split(',') 
                self.combs.append((comb_line[0], keyword, comb_line[1:]))
            elif keyword=='dff': 
                assert line[-1]==';', 'Unexpected line continuation. '
                dff_line = line[line.find('(')+1:-2].split(',')[1:] 
                self.ppis.append(dff_line[0])
                self.ppos.append(dff_line[1])
            pos += 1

def parse(fname): 
    lines = [] 
    fi = open(fname, 'r') 
    for line in fi: 
        lines.append(line)
    fi.close() 

    return Design(lines)

if __name__ == '__main__': 
    if len(sys.argv) != 2: 
        print('USAGE: python v2bench.py <filename>')
        exit(-1) 

    fname = sys.argv[1] 
    design = parse(fname)
    design.write(fname.split('.')[0]+'_C.bench') 
