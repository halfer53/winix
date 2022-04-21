import sys
from os import path, listdir

ENVIRON = "lib/ansi/env.o"
ANSI = "lib/ansi/"
STDLIB = "lib/stdlib/"
GEN = "lib/gen/"
POSIX = "lib/posix/"
SIGNAL = "lib/posix/_sigset.o"
STDIO ="lib/stdio/"
SYSCALL = "lib/syscall/"
STRING = "lib/ansi/string.o"
STRL = "lib/ansi/strl.o"
REGEX = "lib/regex/"
IPC = "lib/ipc/"

built_in = {
	"stdlib.h": set([STDLIB, ANSI]),
	"string.h": set([STRING, STRL]),
	"signal.h": set([SIGNAL]),
	"ucontext.h": set([GEN]),
	"unistd.h": set([POSIX, ANSI, STDLIB]),
	"stdio.h":	set([STDIO, STDLIB]),
	"dirent.h":	set([POSIX, ANSI, STDLIB]),
	"regexp.h": set([REGEX]),
	"sys/ipc.h": set([IPC])
}

def get_all_files(libs):
	ret = set()
	for lib in libs:
		if lib.endswith("/"):
			for file in listdir(lib):
				if file.endswith(".o"):
					ret.add(lib + file)
		else:
			ret.add(lib)
	return ' '.join(ret)

def include_iterate(start, end,line):
	i = 0
	j = 0
	# get header name
	while(line[i] != start):
		i += 1
	i += 1
	j = i
	while(line[j] != end):
		j += 1
	# print(line, i, j, line[i:j])
	return i,j

def get_sys_include(line):
	i,j = include_iterate('<', '>',line)
	if(j > i + 1):
		filename = line[i:j]
		if(filename in built_in):
			# print(built_in[filename])
			return built_in[filename]
	return set()

def get_local_include(line, dir_path):
	i,j = include_iterate('"', '"',line)
	if(j > i + 1):
		filename = line[i:j]
		return do_include_search(dir_path + filename)
	return set()

def do_include_search(filename : str):
	libs = set()
	dir_path = path.dirname(path.realpath(filename)) + '/'
	# print(dir_path)
	if filename.endswith('.o'):
		filename = filename.replace('.o', '.c')
	with open(filename) as f:
		for line in f:
			if "#include" in line:
				tlib = set()
				if "<" in line:
					tlib = get_sys_include(line)
				else:
					tlib = get_local_include(line, dir_path)
				if(len(tlib) > 0):
					libs.update(tlib)
	return libs	

def main():
	if(len(sys.argv) < 2):
		return -1
	libs = {ENVIRON, SYSCALL}
	for i in range(1,len(sys.argv)):
		tlib = do_include_search(sys.argv[i])
		libs.update(tlib)
		
	print(get_all_files(libs), end='')

if __name__ == '__main__':
	main()

