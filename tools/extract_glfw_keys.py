from pathlib import Path
import os

header_file = Path(os.getcwd()) / "external" / "glfw" / "include" / "GLFW" / "glfw3.h"
print(header_file)
codes = {}

with open(header_file, "r") as header:
    lines = header.readlines()
    for line in lines:
        if not line.startswith("#define GLFW_KEY_"):
            continue
        line = line.strip()
        line = line.split()
        name = line[1][5:]
        code = line[2]

        codes[name] = code


with open("output.hpp", "w", encoding='utf-8') as new_header:
    new_header.write("#pragma once\n")
    new_header.write("\n")
    for k,v in codes.items():
        new_header.write(f"#define {k} {v}\n")
    new_header.write("\n")
