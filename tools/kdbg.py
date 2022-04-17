#!/usr/bin/python3
import subprocess
import sys
from subprocess import call
from os import path
from uuid import uuid4
from shutil import rmtree

def main():
    if len(sys.argv) < 2:
        print("Plz provide the PC address when kernel crashed")
        return 0

    main_path = path.dirname(path.realpath(__file__)) + "/.."
    rpath = "include/srec"
    in_file = "winix.verbose"
    
    if len(sys.argv) == 3:
        in_file = sys.argv[2] + ".verbose"
            
    filepath = main_path + "/" + rpath + "/" + in_file
    target = int(sys.argv[1],16)

    prevfile = ""
    target_segment = ""
    target_line_num = 0
    instruction = ""
    addr = 0
    with open( filepath) as f:
        for line in f:
            if "file" in line:
                target_segment = "." + line.split(", .",1)[1]\
                        .split(" : ")[0].replace("\n","")
                prevfile = line
                target_line_num = 1
                if target_segment == ".bss":
                    curr_size = int(line.split(".bss : ")[1].split(" ")[0],16)
                    curr_addr = int(line.split("starting : ")[1]\
                                        .split(", .bss : ")[0],16)
                    if(curr_addr <= target and curr_addr + \
                            curr_size >= target and curr_size != 0):
                        target_line_num = target - curr_addr
                        addr = curr_addr
                        break
                    
            elif len(line) > 0 and line[0] == '0':
                splits = line.split(" ")
                curr_addr = int(splits[0],16)
                instruction = splits[2]
                
                if target == curr_addr:
                    addr = curr_addr
                    break
                target_line_num += 1

    if addr == 0:
        print("Instruction not found 1")
        return 0
    
    filename = prevfile.split(" ")[1].split(", ")[0].replace("'","")\
                    .replace(",","").replace(".o",".c")
    print(f"target assembly line number {target_line_num} in segment {target_segment} in {filename}")

    tmp_filename = "/tmp/" + str(uuid4()) + ".s"
    wcc_cmd = ["wcc","-N", "-g", "-S", "-I" + main_path + "/include/posix_include", "-I" + main_path + "/include",\
                    "-D__wramp__", "-D_DEBUG","-o",tmp_filename, main_path+"/"+filename, ]

    print(" ".join(wcc_cmd))
    result = call(wcc_cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if result != 0:
        tmpfilename = filename.replace(".c",".s")
        if path.isfile(main_path+"/" +  tmpfilename):
            print("Line " + str(target_line_num) + \
                        " in file "+tmpfilename)
            return 0
        else:
            print('Err')
            return 1

    loc = "0"
    curr_count = 0
    next_incr = 0
    prev_name_index = 0
    found = False
    curr_seg = ".data"
    seg_types = [".text", ".data", ".bss"]
    
    with open(tmp_filename, 'r') as f:
        for idx, line in enumerate(f):
            line = line.replace("\n","")
            if not line:
                continue
            # if it's label
            if line[-1] == ':' and "L." not in line:
                prev_name = line
                prev_name_index = 0

            elif line[0] == '.':
                if ".loc" in line:
                    loc = line.split(",")[1]
                elif line in seg_types:
                    if line == ".data" or line == ".bss":
                        curr_count = 0
                    curr_seg = line
                    # print(f"Current progress: {curr_count} in {curr_seg}")

            elif line[0] == '\t':
                if ".asciiz" in line:
                    tmp_str = line.split(".asciiz")[1].strip().replace("\"","")
                    next_incr = len(tmp_str) + 1 # extra one for string terminator
                    # print(".data: "+line)
                    
                elif ".word" in line:
                    next_incr = 1
                elif curr_seg == "":
                    next_incr = 1
                elif curr_seg == ".text":
                    next_incr = 1
                elif curr_seg == ".bss":
                    if ".space" in line:
                        bss_len = int(line.split(".space")[1].strip())
                        # print(prev_line)
                        # print(bss_len)
                        next_incr = bss_len 
                        
                if(curr_count <= target_line_num\
                    and curr_count + next_incr >= target_line_num):
                    print(f"Assembly: \n\t0x{instruction}\n {line}")
                    print(f"Line: {idx} in assembly file")
                    print(f"Line: {loc} in file {filename}:{loc}")
                    found = True
                    break
                # print(str(curr_count), line )
                prev_name_index += 1
                curr_count += next_incr

            prev_line = line

        if not found:
            print("Instruction not found ")

if __name__ == '__main__':
    main()
