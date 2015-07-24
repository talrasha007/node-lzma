#include <nan.h>

#include "lzma.h"
#include "ppmd.h"

using namespace v8;

void init(v8::Handle<v8::Object> exports) {
    Local<Object> lzma = NanNew<Object>();
    LzmaTr::setup(lzma);
    exports->Set(NanNew("lzma"), lzma);

    Local<Object> ppmd = NanNew<Object>();
    PpmdTr::setup(ppmd);
    exports->Set(NanNew("ppmd"), ppmd);

}

NODE_MODULE(lzma, init);