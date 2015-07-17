#include <nan.h>

#include <LzmaLib.h>

using namespace std;
using namespace v8;
using namespace node;

NAN_METHOD(compress) {
    NanScope();

    size_t len = Buffer::Length(args[0]);
    const char *in = Buffer::Data(args[0]);

    unsigned char props[LZMA_PROPS_SIZE];
    size_t propsSize = LZMA_PROPS_SIZE;

    size_t outLen = len + 128 + len / 3;
    char* out = new char[outLen];
    int res = LzmaCompress((unsigned char*)out + propsSize, &outLen, (unsigned char*)in, len, props, &propsSize, 5, (1 << 24), 3, 0, 2, 32, 2);

    Local<Object> ret = NanNewBufferHandle(outLen);
    char *retBuf = Buffer::Data(ret);
    memcpy(retBuf, out, outLen);
    NanReturnValue(ret);
}

NAN_METHOD(decompress) {
    NanScope();

    Local<Object> ret;
    NanReturnValue(ret);
}

void init(v8::Handle<v8::Object> exports) {
    NODE_SET_METHOD(exports, "compress", compress);
    NODE_SET_METHOD(exports, "decompress", decompress);
}

NODE_MODULE(lzma, init);