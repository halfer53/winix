import sys
import subprocess
import os
import glob
import uuid

userfile_dir = {
    "shell.kdbg":"user/",
    "init.kdbg":"init/"
}

def main():
    if(len(sys.argv) < 2):
        print("Plz provide the PC address when kernel crashed")
        return 0

    main_path = os.path.dirname(os.path.realpath(__file__)) + "/.."
    rpath = "tools/kdbg_srec"
    in_file = "winix.kdbg"

    if(len(sys.argv) == 3):
        in_file = sys.argv[2] + ".kdbg"
        if(not os.path.isfile(main_path + "/tools/kdbg_srec/" + in_file)):
            print(in_file + " is not found in tools/kdbg_srec")
            return 0
    
    target = int(sys.argv[1],16)

    prevfile = ""
    linecount = 0
    addr = 0
    with open( rpath + "/" + in_file) as f:
        for line in f:
            if("file" in line):
                prevfile = line
                linecount = 0
            elif(len(line) > 0 and line[0] == '0'):
                curr_addr = int(line.split(" ")[0],16)
                
                if(target == curr_addr):
                    addr = curr_addr
                    break
                linecount += 1

    if(addr == 0):
        print("Instruction not found 1")
        return 0
    
    filename = prevfile.split(" ")[1].replace("'","")\
                    .replace(",","").replace(".o",".c")
    # print(filename)
    # print(linecount)
    # print(in_file)

    tmp_filename = str(uuid.uuid4())

    if(in_file in userfile_dir):
        filename = userfile_dir[in_file] + filename

    wcc_cmd = ["wcc","-N", "-g", "-S", "-I"+main_path+"/include",\
                     "-o",tmp_filename, main_path+"/"+filename, ]

    result = subprocess.call(wcc_cmd)
    if(result != 0):
        tmpfilename = filename.replace(".c",".s")
        if(os.path.isfile(main_path+"/" +  tmpfilename)):
            print("Line " + str(linecount) + \
                        " in file "+tmpfilename)
            return 0

    loc = ""
    curr_count = 0
    in_text_seg = False
    with open(tmp_filename) as f:
        for line in f:

            if(".loc" in line):
                loc = line
            elif(line[0] == '\t'):
                if(curr_count == linecount):
                    print("Line "+ loc[:-1].split(",")[1] + \
                                        " in file "+filename)
                    break
                curr_count += 1

                
    if(curr_count != linecount):
        print("Instruction not found")

    os.remove(tmp_filename)

if __name__ == '__main__':
    main()
