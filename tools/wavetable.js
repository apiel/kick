const fs = require('fs');
const path = require('path');

const NUM_CELLS = 2048;

const [, , target, dist] = process.argv;

if (!target || !dist) {
    console.log(
        'Please provide target file 16 bits signed and destination, e.g: ~/file.raw ./sin.h',
    );
    process.exit();
}

console.log(`Convert ${target} to ${NUM_CELLS} table.`);

if (!fs.existsSync(target)) {
    console.log(`File ${target} does not exist.`);
    process.exit();
}

const content = fs.readFileSync(target);

const len = content.length - 2;
let data = [];
for (let i = 0; i < len; i += 2) {
    data.push(content.readInt16LE(i));
}

if (data.length > NUM_CELLS) {
    data = data.slice(0, NUM_CELLS);
}
console.log(data.join(','));
console.log(data.length);
