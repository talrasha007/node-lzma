#include <iostream>
#include <nan.h>
#include <LzmaLib.h>
#include <LzmaEnc.h>
#include <LzmaDec.h>

#include "buffer_exports.h"
#include "lzma.h"

using namespace std;

void LzmaTr::setup(v8::Handle<v8::Object>& exports) {
    NODE_SET_METHOD(exports, "compress", BufferExports<LzmaTr>::compress);
}

size_t LzmaTr::getUnpackSize(const char *in, size_t len) {
    size_t unpackSize = 0;
    for (int i = 0; i < 8; i++) {
        unpackSize += size_t(Byte(in[LZMA_PROPS_SIZE + i])) << (i * 8);
    }

    if (unpackSize > 2ULL * 1024 * 1024 * 1024) {
        cout << "Unpack size is too large. Bad input or you should try stream api." << endl;
        return 0;
    }

    return unpackSize;
}

int LzmaTr::decompress(char *out, size_t *outLen, const char *in, size_t *inLen) {
    return LzmaUncompress((Byte*)out, outLen, (Byte*)in + LZMA_PROPS_SIZE + 8, inLen, (Byte*)in, LZMA_PROPS_SIZE);
}

int LzmaTr::compress(char *out, size_t *outLen, const char *in, size_t inLen, int level, int threads) {
    size_t propsSize = LZMA_PROPS_SIZE;
    *outLen -= propsSize + 8;

    int r = LzmaCompress((Byte*)out + propsSize + 8, outLen, (Byte*)in, inLen, (Byte*)out, &propsSize, level, 0, -1, -1, -1, -1, threads);
    for (int i = 0; i < 8; i++) {
        out[propsSize++] = char(Byte(*outLen >> (8 * i)));
    }

    return r;
}
