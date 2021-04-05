const fs = require('fs');
const path = require('path');

const NUM_CELLS = 256;

const [, , target, dist] = process.argv;

if (!target || !dist) {
    console.log(
        'Please provide target file and destination, e.g: ~/Mozzi/tables/sin256_int8.h ./sin.h',
    );
    process.exit();
}

console.log(`Convert ${target} to ${NUM_CELLS} table.`);

if (!fs.existsSync(target)) {
    console.log(`File ${target} does not exist.`);
    process.exit();
}

const content = fs.readFileSync(target);
const [data] = content.toString().match(/\{([0-9\s\,\-]+)\}/gs);

if (!data) {
    console.log(`Couldnt find table data.`);
    process.exit();
}
const json = data.replace('{', '[').replace(/([\s\,]+)\}/gs, ']');
const table = JSON.parse(json);
console.log(`Found ${table.length} cells`);
const newTable = [];
const inc = table.length / NUM_CELLS;
console.log(`Increment value ${inc}.`);
for (i = 0, x = 0; i < NUM_CELLS; i++, x += inc) {
    newTable[i] = table[Math.round(x)] << 8;
}

console.log(`New table ${newTable.length} cells`);

const name = path.parse(dist).name.toUpperCase();

const output = `#ifndef ${name}_H_
#define ${name}_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "mozzi_pgmspace.h"

#define ${name}_NUM_CELLS ${NUM_CELLS}
#define ${name}_SAMPLERATE ${NUM_CELLS}

CONSTTABLE_STORAGE(int8_t) ${name}_DATA []  =
        {${newTable.join(',')}};
#endif`;

// console.log('output', output);
fs.writeFileSync(dist, output);
