from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument("f1")
args = parser.parse_args()
def final_ans(file):
    with open(file, 'r') as f:
        lines = f.readlines()
        please_print = False
        print("==========================OUR  ANS===========================")
        for line in lines[:-6]:
            if (line == "#STATS\n"):
                please_print = True
            if (line == "#STATS END\n"):
                please_print = False
            if please_print:
                if line != "#STATS\n" and line != "#STATS END\n":
                    print("..."+line,end="")
        print("=============================================================")
        # print("========================= GOLDEN  ===========================")
        # for line in lines[-6:-1]:
        #     print(line,end="")
        # print("=============================================================")
if __name__ == "__main__":
    final_ans(args.f1)

