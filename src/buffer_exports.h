#pragma once
#ifdef min
#undef min
#endif

#include <algorithm>
#include <nan.h>

template <typename TR> class BufferExports {
public:
    void setup(v8::Handle<v8::Object> exports) {
        NODE_SET_METHOD(exports, "compress", compress);
        NODE_SET_METHOD(exports, "decompress", decompress);
    }

    static NAN_METHOD(compress) {
        NanScope();

        if (args.Length() == 0) {
            NanReturnUndefined();
        }

        int arg1 = -1;
        int arg2 = -1;

        if (args[1]->IsNumber()) { arg1 = args[1]->Int32Value(); }
        if (args[2]->IsNumber()) { arg2 = args[2]->Int32Value(); }

        size_t len = node::Buffer::Length(args[0]);
        const char *in = node::Buffer::Data(args[0]);

        size_t outLen = len + 128 + std::min(size_t(128 * 1024 * 1024), len / 3);
        char *out = new char[outLen];

        TR::compress(out, &outLen, in, len, arg1, arg2);

        v8::Local<v8::Object> ret = NanNewBufferHandle(outLen);
        char *retBuf = node::Buffer::Data(ret);
        memcpy(retBuf, out, outLen);

        delete out;
        NanReturnValue(ret);
    }

    static NAN_METHOD(decompress) {
        NanScope();

        if (args.Length() == 0) {
            NanReturnUndefined();
        }

        size_t len = node::Buffer::Length(args[0]);
        const char *in = node::Buffer::Data(args[0]);

        size_t outLen = TR::getUnpackSize(in, len);
        if (outLen == 0) {
            NanThrowError("Bad input.");
            NanReturnUndefined();
        }
        v8::Local<v8::Object> ret = NanNewBufferHandle(outLen);
        char *out = node::Buffer::Data(ret);

        if (TR::decompress(out, &outLen, in, &len) != 0) {
            NanThrowError("Bad input.");
            NanReturnUndefined();
        }

        NanReturnValue(ret);
    }
};