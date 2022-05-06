import sys
from pathlib import Path
from typing import Dict
import re

def get_all_assemblies(path) -> list[Path]:
	result = list(Path(path).rglob("*.s"))
	return result

def get_assembly_dependencies(filepath):
	export = set()
	required = set()
	with open(filepath) as f:
		PREFIX = '.global'
		JAL = 'jal'
		for line in f:
			if PREFIX in line:
				label = line[:-1].replace(PREFIX, '').strip()
				export.add(label)
		f.seek(0)
		for line in f:
			if JAL in line:
				label = line[:-1].replace(JAL, '').strip()
				if label not in export:
					required.add(label)
			match = re.match(r"\s+lw\s+\$\d+,\s+([^\d]+)\(\$\d+\)", line)
			if match:
				label = match.group(1)
				if label not in export:
					required.add(label)
	return export, required
		
def get_dependencies(libpath: str, files: list[str]) -> str:
	assemblies = get_all_assemblies(libpath)
	export_dict : Dict[str, str] = dict()
	dependency_dict : Dict[str, set] = dict()
	reuqired_files = set()
	for posixpath in assemblies:
		filepath = str(posixpath)
		export, required = get_assembly_dependencies(posixpath.resolve())
		for exp in export:
			export_dict[exp] = filepath
		dependency_dict[filepath] = required
		# print(filepath, export, required, file=sys.stderr)
	for file in files:
		export, required = get_assembly_dependencies(file.replace('.o', '.s'))
		# print(str(file), export, required)
		while required:
			filelist = []
			tmp = set()
			for req in required:
				if req in export_dict:
					dependent_file = export_dict[req]
					reuqired_files.add(dependent_file)
					filelist.append(dependent_file)
			for nextfile in filelist:
				if nextfile in dependency_dict:
					tmp.update(dependency_dict[nextfile])
			required = tmp
	return ' '.join(map(lambda x: x.replace('.s', '.o'), reuqired_files))

def main():
	if(len(sys.argv) < 3):
		return 1
	libpath = sys.argv[1]
	print(get_dependencies(libpath, sys.argv[2:]), end='')

if __name__ == '__main__':
	main()

