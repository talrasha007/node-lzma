var lzma = require('../build/Release/lzma.node'),
    stream = require('./stream.js');

exports.EncStream = stream.EncStream;
exports.DecStream = stream.DecStream;

exports.compress = lzma.compress;
exports.decompress = lzma.decompress;
