#!/usr/bin/env python3
import re

filepath = r'c:\Users\peter\Documents\_Genesis\Game2\src\level.c'
with open(filepath, 'r') as f:
    content = f.read()

match = re.search(r'const u16 map_collision_data\[\] = \s*\{(.*?)\};', content, re.DOTALL)
array_str = match.group(1)
numbers = [int(x.strip()) for x in array_str.split(',') if x.strip().isdigit()]

print(f"Got {len(numbers)} numbers")
assert len(numbers) == 2400, f"Expected 2400, got {len(numbers)}"

old = []
for r in range(40):
    old.append(numbers[r*60:(r+1)*60])

new = []
for r in range(80):
    row = []
    for c in range(120):
        val = old[r//2][c//2]
        row.append(0 if val == 0 else 1)
    new.append(row)

code = "const u16 map_collision_data[] = \n{\n"
for r, row in enumerate(new):
    for c, val in enumerate(row):
        code += str(val)
        if r < 79 or c < 119:
            code += ", "
    code += "\n"
code += "};"

new_content = re.sub(r'const u16 map_collision_data\[\] = \s*\{.*?\};', code, content, flags=re.DOTALL)

with open(filepath, 'w') as f:
    f.write(new_content)

print("Done!")
