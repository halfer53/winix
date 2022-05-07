import sys
from typing import Dict, List, Tuple
import re
import os
import json

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

def get_assembly_dependencies(filepath) -> Tuple[set, set]:
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

def build_dependency_dict(libpath: str) -> Tuple[Dict[str, str], Dict[str, str]]:
    assemblies = get_all_assemblies(libpath)
    dependency_dict : Dict[str, set] = dict()
    export_dict : Dict[str, str] = dict()
    for posixpath in assemblies:
        filepath = str(posixpath)
        export, required = get_assembly_dependencies(posixpath)
        for exp in export:
            export_dict[exp] = filepath
        dependency_dict[filepath] = required
        # print(filepath, export, required, file=sys.stderr)
    return export_dict, dependency_dict

def build_dependency_cache(path: str):
    export_dict, dependency_dict = build_dependency_dict(path)
    obj = {
        'export': export_dict,
        'dependency': dependency_dict
    }
    return obj

def get_dependency_from_cache(libpath: str, cachepath: str) -> Tuple[Dict[str, str], Dict[str, str]]:
    try:
        with open(cachepath) as f:
            obj = json.load(f)
            return obj['export'], obj['dependency']
    except:
        return build_dependency_dict(libpath)
        
def get_dependencies(export_dict : Dict[str, str], dependency_dict : Dict[str, set], files: List[str]) -> str:
    reuqired_files = set()
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

def serialize_sets(obj):
    if isinstance(obj, set):
        return list(obj)
    return obj

def main():
    if(len(sys.argv) < 4):
        return 1
    mode = sys.argv[1]
    libpath = sys.argv[2]
    cachepath = sys.argv[3]
    if mode == 'buildcache':
        obj = build_dependency_cache(libpath)
        with open(cachepath, 'w') as f:
            json.dump(obj, f, default=serialize_sets)
        return
    export_dict, dependency_dict = get_dependency_from_cache(libpath, cachepath)
    if mode == 'linker':
        dependencies = get_dependencies(export_dict, dependency_dict, sys.argv[4:])
        output = ' '.join(map(lambda x: x.replace('.s', '.o'), dependencies))
        print(output, end='')
    elif mode == 'getdependency':
        for file in sys.argv[4:]:
            dependencies = get_dependencies(export_dict, dependency_dict, [file])
            for dep in dependencies:
                print(f"{file}: {dep}")


if __name__ == '__main__':
    main()

