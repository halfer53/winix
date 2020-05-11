#!/usr/bin/python3
import sys
import subprocess
import os
import glob
import uuid
import shutil
import tempfile
from contextlib import contextmanager

userfile_dir = {
    "shell.verbose":"user/",
    "init.verbose":"init/"
}

@contextmanager
def tmpdir():
    path = tempfile.mkdtemp()
    try:
        yield path
    finally:
        try:
            shutil.rmtree(path)
        except IOError:
            sys.stderr.write("fail to clean up temporary directies")

def main():
    if(len(sys.argv) < 2):
        print("Plz provide the PC address when kernel crashed")
        return 0

    main_path = os.path.dirname(os.path.realpath(__file__)) + "/.."
    rpath = "include/srec"
    in_file = "winix.verbose"
    
    if(len(sys.argv) == 3):
        in_file = sys.argv[2] + ".verbose"
            
    filepath = main_path + "/" + rpath + "/" + in_file
    target = int(sys.argv[1],16)

    prevfile = ""
    target_segment = ""
    target_line_num = 0
    addr = 0
    with open( filepath) as f:
        for line in f:
            if("file" in line):
                target_segment = "." + line.split(", .",1)[1]\
                        .split(" : ")[0].replace("\n","")
                prevfile = line
                target_line_num = 0
                if(target_segment == ".bss"):
                    curr_size = int(line.split(".bss : ")[1].split(" ")[0],16)
                    curr_addr = int(line.split("starting : ")[1]\
                                        .split(", .bss : ")[0],16)
                    if(curr_addr <= target and curr_addr + \
                            curr_size >= target and curr_size != 0):
                        target_line_num = target - curr_addr
                        addr = curr_addr
                        break
                    
            elif(len(line) > 0 and line[0] == '0'):
                curr_addr = int(line.split(" ")[0],16)
                
                if(target == curr_addr):
                    addr = curr_addr
                    break
                target_line_num += 1

    if(addr == 0):
        print("Instruction not found 1")
        return 0
    
    filename = prevfile.split(" ")[1].split(", ")[0].replace("'","")\
                    .replace(",","").replace(".o",".c")
    # print(filename)
    # print(target_line_num)
    # print(in_file)
    # print(target_segment)

    with tmpdir() as basedir:
        tmp_filename = basedir + "/" + str(uuid.uuid4())

        # if(in_file in userfile_dir):
        #     filename = userfile_dir[in_file] + filename

        wcc_cmd = ["wcc","-N", "-g", "-S", "-I" + main_path + "/include/posix_include", "-I" + main_path + "/include",\
                        "-D__wramp__", "-D_DEBUG","-o",tmp_filename, main_path+"/"+filename, ]

        print(" ".join(wcc_cmd))
        result = subprocess.call(wcc_cmd, stderr=sys.stderr)
        if(result != 0):
            tmpfilename = filename.replace(".c",".s")
            if(os.path.isfile(main_path+"/" +  tmpfilename)):
                print("Line " + str(target_line_num) + \
                            " in file "+tmpfilename)
                return 0
            else:
                print('Err')
                return 1

        loc = "0"
        curr_count = 0
        next_incr = 0
        in_text_seg = False
        prev_line = ""
        prev_name = ""
        prev_name_index = 0
        curr_seg = ""
        seg_types = [".text\n", ".data\n", ".bss\n"]
        
        with open(tmp_filename, 'r') as f:
            for line in f:
                if(line[-2] == ':' and "L." not in line):
                    prev_name = line
                    prev_name_index = 0

                elif(line[0] == '.'):
                    if(".loc" in line):
                        loc = line[:-1].split(",")[1]
                    elif(line in seg_types):
                        # print(curr_count)
                        if(line == ".data\n" or line == ".bss\n"):
                            curr_count = 0
                        
                        curr_seg = line.replace("\n","")
                        # print("in "+curr_seg)

                elif(line[0] == '\t'):
                    if(curr_seg == ""):
                        next_incr = 1
                        if(curr_count == target_line_num):
                            print(".data: \n" + prev_name + "\tindex: " \
                                + str(prev_name_index) + "\n" + line + "in file " \
                                + filename)
                            break
                    elif(curr_seg == ".text"):
                        next_incr = 1
                        if(curr_seg == target_segment and \
                                curr_count == target_line_num):
                                print("Assembly: \n" + line , end="")
                                print("Line: " + str(curr_count) + " in assembly file")
                                print("Line: "+ loc + " in file "+filename)
                                break
                        
                    elif(curr_seg == ".data"):
                        if(".asciiz" in line):
                            tmp_str = line.split(".asciiz")[1].strip().replace("\"","")
                            
                            next_incr = len(tmp_str)
                            # print(".data: "+line)
                            if(curr_seg == target_segment and curr_count <= target_line_num\
                                    and curr_count + next_incr >= target_line_num):

                                    index_offset = curr_count + next_incr - target_line_num
                                    print(".data: \n\tindex: " + str(index_offset)\
                                             + "\n" + line[:-1] +"\nin file " + filename)
                                    break

                    elif(curr_seg == ".bss"):
                        if(".space" in line):
                            bss_len = int(line.split(".space")[1].strip())
                            # print(prev_line)
                            # print(bss_len)
                            next_incr = bss_len 
                            
                            if(curr_seg == target_segment and curr_count <= target_line_num \
                                    and curr_count + next_incr >= target_line_num):
                                    index_offset = curr_count + next_incr - target_line_num
                                    print(".bss: \n" + prev_line[:-1] + "\n\tindex: " + \
                                        str(index_offset) + "\n\tTotal number of words: " + \
                                        str(bss_len) + "\nin file " + filename)
                                    break
                    print(str(curr_count), line, end='')
                    prev_name_index += 1
                    curr_count += next_incr

                prev_line = line

        if(target_segment == ".text" and curr_count != target_line_num):
            print("Instruction not found ")

if __name__ == '__main__':
    main()
