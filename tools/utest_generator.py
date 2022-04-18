import sys
import re

def get_prototypes(files):
    prototypes = []
    for file in files:
        with open(file) as f:
            for line in f:
                match = re.search(r"\s*void\s+test_(\w+)\s*\(\s*\)", line)
                if match:
                    prototypes.append(f"test_{match.group(1)}")
    return prototypes

def generate(prototypes):
    print("#include <sys/fcntl.h>")
    print("#include <stdio.h>")
    print()
    for proto in prototypes:
        print(f"void {proto}();")
    print()
    print("void run_all_tests(){")
    for proto in prototypes:
        print(f"    printf(\"%s\\n\", \"running {proto}\");")
        print(f"    {proto}();")
        print(f"    printf(\"%s\\n\\n\", \"passed: {proto}\");")
        print()
    print("}")

def main():
    if len(sys.argv) < 2:
        exit(1)
    files = sys.argv[1:]
    prototypes = get_prototypes(files)
    generate(prototypes)

if __name__ == '__main__':
    main()