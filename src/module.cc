#include <memory>
#include <vector>
#include <nan.h>
#include <Alloc.h>
#include <LzmaLib.h>
#include <LzmaEnc.h>

using namespace std;
using namespace v8;
using namespace node;

class LzmaEnc : public node::ObjectWrap {
public:
    static void setup(Handle<Object>& exports) {
        const char* className = "LzmaEnc";
        
        Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(LzmaEnc::ctor);
        tpl->SetClassName(NanNew(className));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        
        NODE_SET_PROTOTYPE_METHOD(tpl, "push", LzmaEnc::push);
        NODE_SET_PROTOTYPE_METHOD(tpl, "destroy", LzmaEnc::destroy);
        
        exports->Set(NanNew(className), tpl->GetFunction());
    }
    
private:
    static NAN_METHOD(ctor) {
        NanScope();
        
        int level = 5;
        int threads = 1;
        
        if (args[0]->IsNumber()) {
            int argLvl = args[0]->Int32Value();
            if (argLvl >= 0 && argLvl <= 9) level = argLvl;
        }
        
        if (args[1]->IsNumber()) {
            int argTh = args[1]->Int32Value();
            if (argTh == 1 || argTh == 2) threads= argTh;
        }
        
        LzmaEnc *ptr = new LzmaEnc(level, threads);
        ptr->Wrap(args.This());
        NanReturnValue(args.This());
    }
    
    static NAN_METHOD(push) {
        NanScope();
        
        size_t srcLen = Buffer::Length(args[0]);
        char *src = Buffer::Data(args[0]);
        
        size_t dstLen = srcLen + 128 + srcLen / 3;
        char *dst = new char[dstLen];
        
        LzmaEnc *ptr = ObjectWrap::Unwrap<LzmaEnc>(args.This());
        LzmaEnc_MemEncode(ptr->_handle, (Byte*)dst, &dstLen, (Byte*)src, srcLen,
                          0, NULL, &g_Alloc, &g_Alloc);
        
        
        Local<Object> ret = NanNewBufferHandle(dstLen);
        char *retBuf = Buffer::Data(ret);
        memcpy(retBuf, dst, dstLen);
        
        delete dst;
        NanReturnValue(ret);
    }
    
    static NAN_METHOD(destroy) {
        NanScope();
        
        LzmaEnc *ptr = ObjectWrap::Unwrap<LzmaEnc>(args.This());
        LzmaEnc_Destroy(ptr->_handle, &g_Alloc, &g_Alloc);
        ptr->_handle = NULL;
        
        NanReturnUndefined();
    }
    
private:
    LzmaEnc(int level, int threads) : _propBufSz(LZMA_PROPS_SIZE) {
        LzmaEncProps_Init(&_props);
        _props.level = level;
        _props.numThreads = threads;
        
        _handle = LzmaEnc_Create(&g_Alloc);
        
        LzmaEnc_SetProps(_handle, &_props);
        LzmaEnc_WriteProperties(_handle, _propBuf, &_propBufSz);
    }
    
private:
    CLzmaEncHandle _handle;
    CLzmaEncProps _props;
    
    Byte _propBuf[LZMA_PROPS_SIZE];
    size_t _propBufSz;
};

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
    
    LzmaEnc::setup(exports);
}

NODE_MODULE(lzma, init);