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
        if (info.Length() == 0) {
            return ;
        }

        int arg1 = -1;
        int arg2 = -1;

        if (info[1]->IsNumber()) { arg1 = info[1]->Int32Value(); }
        if (info[2]->IsNumber()) { arg2 = info[2]->Int32Value(); }

        size_t len = node::Buffer::Length(info[0]);
        const char *in = node::Buffer::Data(info[0]);

        size_t outLen = len + 128 + std::min(size_t(128 * 1024 * 1024), len / 3);
        char *out = new char[outLen];

        TR::compress(out, &outLen, in, len, arg1, arg2);

        v8::Local<v8::Object> ret = Nan::CopyBuffer(out, outLen).ToLocalChecked();

        delete out;
        info.GetReturnValue().Set(ret);
    }

    static NAN_METHOD(decompress) {
        if (info.Length() == 0) {
            return ;
        }

        size_t len = node::Buffer::Length(info[0]);
        const char *in = node::Buffer::Data(info[0]);

        size_t outLen = TR::getUnpackSize(in, len);
        if (outLen == 0) {
            Nan::ThrowError("Bad input.");
            return ;
        }

        char *out = new char[outLen];
        if (TR::decompress(out, &outLen, in, &len) != 0) {
            Nan::ThrowError("Bad input.");
            return ;
        }

        v8::Local<v8::Object> ret = Nan::NewBuffer(out, outLen).ToLocalChecked();
        info.GetReturnValue().Set(ret);
    }
};