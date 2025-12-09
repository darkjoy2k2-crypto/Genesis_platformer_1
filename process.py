import re

# Read the file
with open('c:\\Users\\peter\\Documents\\_Genesis\\Game2\\src\\level.c', 'r') as f:
    content = f.read()

# Find and extract the array data
match = re.search(r'const u16 map_collision_data\[\] = \s*\{(.*?)\};', content, re.DOTALL)
if not match:
    print("ERROR: Could not find array")
    exit(1)

# Extract and parse the numbers
array_str = match.group(1)
numbers = []
for num_str in array_str.split(','):
    num_str = num_str.strip()
    if num_str:
        numbers.append(int(num_str))

print(f"Found {len(numbers)} values")
print(f"Expected 60x40={60*40} values")

if len(numbers) != 2400:
    print(f"ERROR: Expected 2400 values but got {len(numbers)}")
    exit(1)

# Convert to 2D (40 rows x 60 cols)
old_data = []
for r in range(40):
    row = numbers[r*60:(r+1)*60]
    old_data.append(row)

# Create scaled 80x120 array (each cell becomes 2x2, all non-zero -> 1)
new_data = []
for r in range(80):
    new_row = []
    old_row = r // 2
    for c in range(120):
        old_col = c // 2
        old_val = old_data[old_row][old_col]
        new_val = 0 if old_val == 0 else 1
        new_row.append(new_val)
    new_data.append(new_row)

# Generate C code
output = "const u16 map_collision_data[] = \n{\n"
for r in range(80):
    for c in range(120):
        output += str(new_data[r][c])
        if r < 79 or c < 119:
            output += ", "
output += "\n};"

# Replace in content
new_content = re.sub(
    r'const u16 map_collision_data\[\] = \s*\{.*?\};',
    output,
    content,
    flags=re.DOTALL
)

# Write back
with open('c:\\Users\\peter\\Documents\\_Genesis\\Game2\\src\\level.c', 'w') as f:
    f.write(new_content)

print("Successfully updated level.c with 120x80 array")
print("All non-zero values converted to 1")
