import sys
from pathlib import Path
from typing import Dict, List
import re
import os

def run_fast_scandir(dir, ext):    # dir: str, ext: list
    dirname = os.getcwd() + '/'
    subfolders, files = [], []
    for f in os.scandir(dir):
        if f.is_dir():
            subfolders.append(f.path)
        if f.is_file():
            if os.path.splitext(f.name)[1].lower() in ext:
                filepath = f.path.replace(dirname, '')
                # print(f"add {filepath} under {dirname}", file=sys.stderr)
                files.append(filepath)

    for dir in list(subfolders):
        sf, f = run_fast_scandir(dir, ext)
        subfolders.extend(sf)
        files.extend(f)
    return subfolders, files

def get_all_assemblies(path) -> List[str]:
    _, files = run_fast_scandir(path, ['.s'])
    return files

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
        
def get_dependencies(libpath: str, files: List[str]) -> str:
    assemblies = get_all_assemblies(libpath)
    export_dict : Dict[str, str] = dict()
    dependency_dict : Dict[str, set] = dict()
    reuqired_files = set()
    for posixpath in assemblies:
        filepath = str(posixpath)
        export, required = get_assembly_dependencies(posixpath)
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
    return reuqired_files

def main():
    if(len(sys.argv) < 4):
        return 1
    mode = sys.argv[1]
    libpath = sys.argv[2]
    if mode == 'linker':
        dependencies = get_dependencies(libpath, sys.argv[3:])
        output = ' '.join(map(lambda x: x.replace('.s', '.o'), dependencies))
        print(output, end='')
    elif mode == 'getdependency':
        for file in sys.argv[3:]:
            dependencies = get_dependencies(libpath, [file])
            for dep in dependencies:
                print(f"{file}: {dep}")


if __name__ == '__main__':
    main()

