import re
import sys

filepath = r'c:\Users\peter\Documents\_Genesis\Game2\src\level.c'

with open(filepath, 'r') as f:
    content = f.read()

match = re.search(r'const u16 map_collision_data\[\] = \s*\{(.*?)\};', content, re.DOTALL)
array_str = match.group(1).strip()

# Parse all numbers
numbers = [int(x.strip()) for x in array_str.split(',') if x.strip()]

# Reshape to 40x60
old_data = [numbers[i*60:(i+1)*60] for i in range(40)]

# Scale to 80x120
new_data = []
for r in range(80):
    row = []
    for c in range(120):
        val = old_data[r//2][c//2]
        row.append(0 if val == 0 else 1)
    new_data.append(row)

# Build new array code
code = "const u16 map_collision_data[] = \n{\n"
for r, row in enumerate(new_data):
    for c, val in enumerate(row):
        code += str(val)
        if r < 79 or c < 119:
            code += ", "
    code += "\n"
code += "};"

# Replace
new_content = re.sub(r'const u16 map_collision_data\[\] = \s*\{.*?\};', code, content, flags=re.DOTALL)

with open(filepath, 'w') as f:
    f.write(new_content)

print("Done!")
