"use strict";

var lzma = require('../');

var buf = require('fs').readFileSync(__dirname + '/stream.js');

console.log('Origin: ', buf.length, buf);

var compressed = lzma.compress(buf);
console.log('Compressed: ', compressed.length, compressed);

//var decompressed = lzma.decompress(compressed);
//console.log('Decompressed: ', decompressed);

require('fs').writeFileSync(__dirname + '/test.lzma', compressed);