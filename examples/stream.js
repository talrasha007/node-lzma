"use strict";

var fs = require('fs'),
    lzma = require('../'),
    EncStream = lzma.EncStream,
    DecStream = lzma.DecStream;

var input = fs.createReadStream(__dirname + '/stream.js');
input.pipe(new EncStream).pipe(fs.createWriteStream(__dirname + '/test.lzma'));

input.on('end', function () {
    input.close();
    input = fs.createReadStream(__dirname + '/test.lzma');
    input.pipe(new DecStream).pipe(fs.createWriteStream(__dirname + '/test.txt'));
});