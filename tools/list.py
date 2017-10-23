import sys
import re

def replace_line(line):
    line = line.replace("struct list_head *","")\
        .replace("struct hlist_node *","").replace("struct hlist_head *","")\
        .replace("const","")
    return line

def next_prev(line):
    tmpline = re.sub("([^>])next", r'\1' +"nextl", line)
    tmpline = re.sub("([^>])prev", r'\1' +"prevl", tmpline)
    return tmpline

def process_line(line):
    return next_prev(replace_line(line))[:-1]

def main():
    output = []
    with open(sys.argv[1]) as f:
        inblock = False
        inDef = False
        for line in f:
            if(line.startswith("static inline")):
                defline = re.sub("static inline ([^ ]+)(.+)\n", r'\1' + r'\2' , line)
                output.append(defline + ";\n")
            line = re.sub("static inline [^ ]+", "#define  ", line)
            params = []
            if("#define  " in line):
                output.append(process_line(line) + "\\\n")
                inDef = True

            elif(line == "{\n"):
                output.append("do{\\\n")
                inDef = False
                inblock = True

            elif(line == "}\n"):
                output.append("}while(0)\n")
                inblock = False
            
            elif(inblock == True):
                tmpline = process_line(line)
                tmpline = re.sub("([\w_]+)->","("+ r'\1' +")->", tmpline)
                output.append(tmpline + "\\\n")

            elif(inDef == True):
                tmpline = process_line(line)
                output.append(tmpline + "\\\n")
            else:
                output.append(line)

    for line in output:
        print(line, end='')

main()