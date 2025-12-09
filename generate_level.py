#!/usr/bin/env python3
"""Generate scaled collision map for Game2"""

import sys
import os

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Step 1: Parse the existing level.c file
filepath = os.path.join(os.path.dirname(__file__), 'src', 'level.c')

try:
    with open(filepath, 'r', encoding='utf-8') as f:
        original_content = f.read()
    print("[OK] Read level.c")
except Exception as e:
    print(f"[ERROR] Could not read level.c: {e}")
    sys.exit(1)

# Step 2: Extract the array data
import re
match = re.search(r'const u16 map_collision_data\[\] = \s*\{(.*?)\};', original_content, re.DOTALL)
if not match:
    print("[ERROR] Could not find map_collision_data in level.c")
    sys.exit(1)

array_str = match.group(1)
numbers = []
for token in array_str.split(','):
    token = token.strip()
    if token and token.isdigit():
        numbers.append(int(token))

print(f"[OK] Parsed {len(numbers)} values")

if len(numbers) != 2400:
    print(f"[WARN] Expected 2400 values (60x40) but got {len(numbers)}")

# Step 3: Reshape to 2D (40 rows x 60 columns)
rows_in = 40
cols_in = 60
old_data = []
for r in range(rows_in):
    row = numbers[r * cols_in : (r + 1) * cols_in]
    if len(row) != cols_in:
        print(f"[WARN] Row {r} has {len(row)} values, expected {cols_in}")
    old_data.append(row)

print(f"[OK] Reshaped to {len(old_data)}x{len(old_data[0]) if old_data else 0}")

# Step 4: Scale to 120x80 with conversion (0→0, other→1)
rows_out = 80
cols_out = 120
new_data = []
for r in range(rows_out):
    row = []
    src_r = r // 2
    for c in range(cols_out):
        src_c = c // 2
        src_val = old_data[src_r][src_c]
        new_val = 0 if src_val == 0 else 1
        row.append(new_val)
    new_data.append(row)

print(f"[OK] Scaled to {len(new_data)}x{len(new_data[0])}")

# Step 5: Generate C code
code_lines = ["const u16 map_collision_data[] = ", "{"]
for r in range(rows_out):
    line = ""
    for c in range(cols_out):
        line += str(new_data[r][c])
        if r < rows_out - 1 or c < cols_out - 1:
            line += ", "
    code_lines.append(line)
code_lines.append("};")

new_array_code = "\n".join(code_lines)

# Step 6: Replace in original content
new_content = re.sub(
    r'const u16 map_collision_data\[\] = \s*\{.*?\};',
    new_array_code,
    original_content,
    flags=re.DOTALL
)

# Step 7: Write to file
try:
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(new_content)
    print(f"[OK] Wrote updated level.c")
except Exception as e:
    print(f"[ERROR] Could not write level.c: {e}")
    sys.exit(1)

print("[OK] Successfully transformed array from 60x40 to 120x80")
print("[OK] All non-zero values converted to 1, zeros remain 0")
