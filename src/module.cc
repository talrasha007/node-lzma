#include <nan.h>

#include "lzma.h"

using namespace v8;

void init(v8::Handle<v8::Object> exports) {
    Local<Object> lzma = NanNew<Object>();
    LzmaTr::setup(lzma);
    exports->Set(NanNew("lzma"), lzma);
}

NODE_MODULE(lzma, init);