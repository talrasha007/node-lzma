#include <nnu.h>

#include "lzma.h"
#include "ppmd.h"

using namespace v8;

NAN_MODULE_INIT(InitAll) {
    Local<Object> lzma = Nan::New<Object>();
    LzmaTr::setup(lzma);
    target->Set(nnu::newString("lzma"), lzma);

    Local<Object> ppmd = Nan::New<Object>();
    PpmdTr::setup(ppmd);
    target->Set(nnu::newString("ppmd"), ppmd);

}

NODE_MODULE(lzma, InitAll);