#!/usr/bin/env python3
import re

# Read level.c
with open('src/level.c', 'r') as f:
    content = f.read()

# Extract the array
match = re.search(r'const u16 map_collision_data\[\] = \{(.*?)\};', content, re.DOTALL)
if not match:
    print("Could not find array in level.c")
    exit(1)

array_str = match.group(1).strip()

# Parse numbers
numbers = [int(x.strip()) for x in array_str.split(',')]
print(f"Total values: {len(numbers)}")
print(f"Expected 60x40: {60*40}")

# Convert to 2D array (40 rows, 60 cols)
rows = 40
cols = 60
old_array = []
for r in range(rows):
    row = []
    for c in range(cols):
        row.append(numbers[r * cols + c])
    old_array.append(row)

# Scale up: each old cell becomes 2x2 in new array
new_rows = 80
new_cols = 120
new_array = []
for r in range(new_rows):
    row = []
    for c in range(new_cols):
        old_r = r // 2
        old_c = c // 2
        val = old_array[old_r][old_c]
        # Convert: 0 stays 0, everything else becomes 1
        row.append(0 if val == 0 else 1)
    new_array.append(row)

# Generate new array declaration
output = "const u16 map_collision_data[] = \n{"
for r in range(new_rows):
    output += "\n"
    for c in range(new_cols):
        output += str(new_array[r][c])
        if r < (new_rows - 1) or c < (new_cols - 1):
            output += ", "

output += "\n};"

# Read level.c and replace the array
with open('src/level.c', 'r') as f:
    content = f.read()

# Replace the entire map_collision_data array
new_content = re.sub(
    r'const u16 map_collision_data\[\] = \{.*?\};',
    output,
    content,
    flags=re.DOTALL
)

# Write back
with open('src/level.c', 'w') as f:
    f.write(new_content)

print("Successfully scaled array from 60x40 to 120x80")
print(f"Array has {new_rows} rows and {new_cols} columns")
