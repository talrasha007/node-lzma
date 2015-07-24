# node-lzma
  Node.js bindings for the xz compression library.  

# Why i write this package
  - It's native, performance is almost the same as c version.
  - I need ppmd algorithm.

# API
```
/* Stream API is WIP. */

var liblzma = require('node-lzma');

var buf = new Buffer(...);
...blablabal...

// LZMA
var lzma = liblzma.lzma;
var compressedDefault = lzma.compress(buf);
var compressed = lzma.compress(buf, level, threads); // level: 0-9, threads: 1-2, default: { level: 5, threads: 1 }
var decompressed = lzma.decompress(compressed);

// PPMD
var ppmd = liblzma.ppmd;
var compressedDefault = ppmd.compress(buf);
var compressed = ppmd.compress(buf, order, dictSizeMB); // order: 2-64, dictSizeMB: 2 - 2047, default: { order: 8, dictSizeMB: 8 }
var decompressed = ppmd.decompress(compressed);
```
