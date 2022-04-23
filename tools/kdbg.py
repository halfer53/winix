#!/usr/bin/python3
import subprocess
import sys
from subprocess import call
from os import path
from uuid import uuid4

def main():
    if len(sys.argv) < 3:
        print("python tools/kdby.py <file> <vir_address>")
        return 1

    main_path = path.dirname(path.realpath(__file__)) + "/.."
    rpath = "include_winix/srec"

    in_file = sys.argv[1] + ".verbose"
    target = int(sys.argv[2],16)

    verbose_filepath = rpath + "/" + in_file
    verbose_fullpath = main_path + "/" + verbose_filepath

    verbose_line_no = 0
    prevfile = ""
    target_segment = ""
    target_line_num = 0
    instruction = ""
    addr = 0
    with open( verbose_fullpath) as f:
        for line_nr, line in enumerate(f):
            verbose_line_no = line_nr + 1
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

    tmp_filename = "/tmp/" + filename.replace("/", "_") + ".s"
    wcc_cmd = ["wcc","-N", "-g", "-S", "-I" + main_path + "/include", "-I" + main_path + "/include_winix",\
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
                    curr_seg = line
                    # print(f"Current progress: {curr_count} in {curr_seg}")

            elif line[0] == '\t':
                if ".asciiz" in line:
                    tmp_str = line.split(".asciiz")[1].strip().replace("\"","")
                    next_incr = len(tmp_str) + 1 # extra one for string terminator
                    curr_seg = ".data"
                    
                elif ".word" in line:
                    next_incr = 1
                    curr_seg = ".bss"
                elif curr_seg == "":
                    next_incr = 1
                elif curr_seg == ".text":
                    next_incr = 1
                elif curr_seg == ".bss":
                    if ".space" in line:
                        bss_len = int(line.split(".space")[1].strip())
                        next_incr = bss_len 
                        
                if(curr_count <= target_line_num\
                    and curr_count + next_incr >= target_line_num):
                    print(f"Assembly: \n\t0x{instruction}\n {line}")
                    print(f"Line: {idx} in {tmp_filename}:{idx}")
                    if curr_seg == ".text":
                        print(f"Line: {loc} in {filename}:{loc}")
                    print(f"Verbose: {verbose_line_no} in {verbose_filepath}:{verbose_line_no}")
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
