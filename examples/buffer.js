"use strict";

var liblzma = require('../');

var buf = require('fs').readFileSync(__dirname + '/stream.js');

console.log('Origin: ', buf.length, buf);

var alg = ['lzma', 'ppmd'];

alg.forEach(function (a) {
    console.log('Using alg: ', a);
    var compressed = liblzma[a].compress(buf);
    console.log('Compressed: ', compressed.length, compressed);

    var decompressed = liblzma[a].decompress(compressed);
    console.log('Decompressed: ', decompressed.length, decompressed);

    if (decompressed.equals(buf)) {
        console.log('Data is exactly the same...');
    } else {
        console.log('Opps....');
    }
});
