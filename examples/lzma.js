"use strict";

var lzma = require('../');

var buf = new Buffer('abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789');

console.log('Origin: ', buf.length, buf);

var compressed = lzma.compress(buf);
console.log('Compressed: ', compressed.length, compressed);

var decompressed = lzma.decompress(compressed);
console.log('Decompressed: ', decompressed);