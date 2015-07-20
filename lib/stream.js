"use strict";

var util = require('util'),
    Transform = require('stream').Transform,
    lzma = require('../build/Release/lzma.node'),
    LzmaEnc = lzma.LzmaEnc,
    LzmaDec = lzma.LzmaDec;

function EncStream(opt) {
    opt = opt || {};
    this._handle = new LzmaEnc(opt.level || opt.lvl || 5, opt.threads || 2);
    Transform.call(this);
}

util.inherits(EncStream, Transform);

EncStream.prototype._transform  = function (data, encoding, done) {
    var outData = this._handle.push(data);
    this.push(outData);
    done();
};

function DecStream() {
    this._handle = new LzmaDec();
    Transform.call(this);
}

util.inherits(DecStream, Transform);

DecStream.prototype._transform = function (data, encoding, done) {
    for (var pos = 0; pos < data.length;) {
        var pos = this._handle.push(data, pos);
        this.push(this._handle.getUncompressed());
    }

    done();
};

exports.EncStream = EncStream;
exports.DecStream = DecStream;