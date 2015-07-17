var lzma = require('../build/Release/lzma.node');

exports.compress = lzma.compress;
exports.decompress = lzma.decompress;