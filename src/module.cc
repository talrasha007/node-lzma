#include <memory>
#include <vector>
#include <nan.h>
#include <LzmaLib.h>

using namespace std;
using namespace v8;
using namespace node;

NAN_METHOD(compress) {
    NanScope();

    if (args.Length() == 0) {
        NanReturnUndefined();
    }

    int level = 5;
    int threads = 1;

    if (args[1]->IsNumber()) {
        int argLvl = args[1]->Int32Value();
        if (argLvl >= 0 && argLvl <= 9) level = argLvl;
    }

    if (args[2]->IsNumber()) {
        int argTh = args[2]->Int32Value();
        if (argTh == 1 || argTh == 2) threads= argTh;
    }

    size_t len = Buffer::Length(args[0]);
    const char *in = Buffer::Data(args[0]);

    unsigned char props[LZMA_PROPS_SIZE];
    size_t propsSize = LZMA_PROPS_SIZE;

    size_t outLen = len + 128 + len / 3;
    char *out = new char[outLen];
    LzmaCompress((unsigned char*)out + propsSize, &outLen, (unsigned char*)in, len, props, &propsSize, level, 0, -1, -1, -1, -1, threads);

    Local<Object> ret = NanNewBufferHandle(outLen);
    char *retBuf = Buffer::Data(ret);
    memcpy(retBuf, out, outLen);

    delete out;
    NanReturnValue(ret);
}

void init(v8::Handle<v8::Object> exports) {
    NODE_SET_METHOD(exports, "compress", compress);
}

NODE_MODULE(lzma, init);