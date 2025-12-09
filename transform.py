#!/usr/bin/env python3
import re
import sys

def main():
    filepath = r'c:\Users\peter\Documents\_Genesis\Game2\src\level.c'
    
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading file: {e}")
        return 1
    
    # Extract the array
    match = re.search(r'const u16 map_collision_data\[\] = \s*\{(.*?)\};', content, re.DOTALL)
    if not match:
        print("Could not find map_collision_data array")
        return 1
    
    # Parse the numbers
    array_str = match.group(1).strip()
    numbers = []
    for item in array_str.split(','):
        item = item.strip()
        if item:
            try:
                numbers.append(int(item))
            except ValueError:
                print(f"Could not parse: {item}")
                continue
    
    print(f"Parsed {len(numbers)} values")
    if len(numbers) != 2400:
        print(f"ERROR: Expected 2400 values (60x40), got {len(numbers)}")
        return 1
    
    # Convert to 2D array (40 rows, 60 columns)
    old_array = []
    for r in range(40):
        row = numbers[r*60:(r+1)*60]
        old_array.append(row)
    
    # Scale to 80x120 (each cell becomes 2x2)
    # With conversion: 0 stays 0, all other values become 1
    new_array = []
    for r in range(80):
        new_row = []
        old_r = r // 2
        for c in range(120):
            old_c = c // 2
            old_val = old_array[old_r][old_c]
            new_val = 0 if old_val == 0 else 1
            new_row.append(new_val)
        new_array.append(new_row)
    
    # Generate new C code
    new_array_code = "const u16 map_collision_data[] = \n{"
    for r in range(80):
        new_array_code += "\n"
        for c in range(120):
            new_array_code += str(new_array[r][c])
            if r < 79 or c < 119:
                new_array_code += ", "
    new_array_code += "\n};"
    
    # Replace in content
    new_content = re.sub(
        r'const u16 map_collision_data\[\] = \s*\{.*?\};',
        new_array_code,
        content,
        flags=re.DOTALL
    )
    
    # Write back
    try:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"Successfully updated {filepath}")
        print(f"Scaled from 60x40 (2400 values) to 120x80 (9600 values)")
        return 0
    except Exception as e:
        print(f"Error writing file: {e}")
        return 1

if __name__ == '__main__':
    sys.exit(main())
