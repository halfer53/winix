import sys
import re

def get_prototypes(files):
    prototypes = []
    for file in files:
        with open(file) as f:
            content = f.read()
            match = re.findall(r"^\s*void\s+test_(\w+)\s*\(\s*\)\s*{", content, flags=re.MULTILINE)
            for proto in match:
                proto = f"test_{proto}"
                prototypes.append(proto)
    return prototypes

def generate(prototypes):
    print("#include <sys/fcntl.h>")
    print("#include <stdio.h>")
    print("#include \"unit_test.h\"")
    print()
    for proto in prototypes:
        print(f"void {proto}();")
    print()
    print("void run_all_tests(){")
    for proto in prototypes:
        print(f"    printf(\"%s\\n\", \"running {proto}\");")
        print(f"    reset_fs();")
        print(f"    {proto}();")
        print(f"    printf(\"%s\\n\\n\", \"passed: {proto}\");")
        print()
    print(f"    printf(\"%d tests passed\\n\", {len(prototypes)});")
    print("}")

def main():
    if len(sys.argv) < 2:
        exit(1)
    files = sys.argv[1:]
    prototypes = get_prototypes(files)
    generate(prototypes)

if __name__ == '__main__':
    main()