const fs = require('fs');
const path = require('path');

// Read level.c
const filePath = path.join(__dirname, 'src', 'level.c');
let content = fs.readFileSync(filePath, 'utf8');

// Extract the array
const match = content.match(/const u16 map_collision_data\[\] = \{(.*?)\};/s);
if (!match) {
    console.error('Could not find array in level.c');
    process.exit(1);
}

const arrayStr = match[1].trim();

// Parse numbers
const numbers = arrayStr.split(',').map(x => parseInt(x.trim()));
console.log(`Total values: ${numbers.length}`);
console.log(`Expected 60x40: ${60*40}`);

// Convert to 2D array (40 rows, 60 cols)
const rows = 40;
const cols = 60;
const oldArray = [];
for (let r = 0; r < rows; r++) {
    const row = [];
    for (let c = 0; c < cols; c++) {
        row.push(numbers[r * cols + c]);
    }
    oldArray.push(row);
}

// Scale up: each old cell becomes 2x2 in new array
const newRows = 80;
const newCols = 120;
const newArray = [];
for (let r = 0; r < newRows; r++) {
    const row = [];
    for (let c = 0; c < newCols; c++) {
        const oldR = Math.floor(r / 2);
        const oldC = Math.floor(c / 2);
        const val = oldArray[oldR][oldC];
        // Convert: 0 stays 0, everything else becomes 1
        row.push(val === 0 ? 0 : 1);
    }
    newArray.push(row);
}

// Generate new array declaration
let output = 'const u16 map_collision_data[] = \n{';
for (let r = 0; r < newRows; r++) {
    output += '\n';
    for (let c = 0; c < newCols; c++) {
        output += newArray[r][c];
        if (r < (newRows - 1) || c < (newCols - 1)) {
            output += ', ';
        }
    }
}
output += '\n};';

// Replace the entire map_collision_data array
const newContent = content.replace(
    /const u16 map_collision_data\[\] = \{.*?\};/s,
    output
);

// Write back
fs.writeFileSync(filePath, newContent, 'utf8');

console.log('Successfully scaled array from 60x40 to 120x80');
console.log(`Array has ${newRows} rows and ${newCols} columns`);
