#include <memory>
#include <vector>
#include <nan.h>
#include <Alloc.h>
#include <LzmaLib.h>
#include <LzmaEnc.h>
#include <LzmaDec.h>

#include "lzma.h"

using namespace std;
using namespace v8;
using namespace node;

class LzmaDec : public ObjectWrap {
public:
    static void setup(Handle<Object>& exports) {
        const char* className = "LzmaDec";

        Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(LzmaDec::ctor);
        tpl->SetClassName(NanNew(className));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        NODE_SET_PROTOTYPE_METHOD(tpl, "push", LzmaDec::push);
        NODE_SET_PROTOTYPE_METHOD(tpl, "getUncompressed", LzmaDec::getUncompressed);
        NODE_SET_PROTOTYPE_METHOD(tpl, "destroy", LzmaDec::destroy);

        exports->Set(NanNew(className), tpl->GetFunction());
    }

private:
    static NAN_METHOD(ctor) {
        NanScope();

        LzmaDec *ptr = new LzmaDec();
        ptr->Wrap(args.This());
        NanReturnValue(args.This());
    }

    static NAN_METHOD(push) {
        NanScope();

        size_t srcPos = args[1]->Uint32Value();
        size_t srcLen = Buffer::Length(args[0]) - srcPos;
        char *src = Buffer::Data(args[0]);

        LzmaDec *ptr = ObjectWrap::Unwrap<LzmaDec>(args.This());
        if (ptr->_propBufSize < sizeof(ptr->_propBuf)) {
            for (; srcLen > 0 && ptr->_propBufSize < sizeof(ptr->_propBuf);) {
                ptr->_propBuf[ptr->_propBufSize++] = src[srcPos++];
                --srcLen;
            }

            if (ptr->_propBufSize == sizeof(ptr->_propBuf)) {
                LzmaDec_AllocateProbs(&ptr->_handle, ptr->_propBuf, ptr->_propBufSize, &g_Alloc);

                ptr->_handle.dic = ptr->_outBuf;
                ptr->_handle.dicBufSize = sizeof(ptr->_outBuf);
                LzmaDec_Init(&ptr->_handle);
            } else {
                NanReturnValue(NanNew(double(srcPos + srcLen)));
            }
        }

        if (srcLen > 0) {
            ELzmaStatus status = LZMA_STATUS_NOT_SPECIFIED;
            auto res = LzmaDec_DecodeToDic(&ptr->_handle, sizeof(ptr->_outBuf), (Byte*)src + srcPos, &srcLen, LZMA_FINISH_ANY, &status);
        }

        NanReturnValue(NanNew(double(srcPos + srcLen)));
    }

    static NAN_METHOD(getUncompressed) {
        NanScope();

        LzmaDec *ptr = ObjectWrap::Unwrap<LzmaDec>(args.This());
        Local<Object> ret = NanNewBufferHandle(ptr->_handle.dicPos);
        char *retBuf = Buffer::Data(ret);
        memcpy(retBuf, ptr->_outBuf, ptr->_handle.dicPos);
        ptr->_handle.dicPos = 0;

        NanReturnValue(ret);
    }

    static NAN_METHOD(destroy) {
        NanScope();

        LzmaDec *ptr = ObjectWrap::Unwrap<LzmaDec>(args.This());
        LzmaDec_FreeProbs(&ptr->_handle, &g_Alloc);

        NanReturnUndefined();
    }

private:
    LzmaDec() : _propBufSize(0) {
        LzmaDec_Construct(&_handle);
    }

private:
    CLzmaDec _handle;

    size_t _propBufSize;
    Byte _propBuf[LZMA_PROPS_SIZE];
    Byte _outBuf[8192];
};

class LzmaEnc : public ObjectWrap {
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
        
        size_t retLen = ptr->_firstPass ? dstLen + ptr->_propBufSz : dstLen;
        Local<Object> ret = NanNewBufferHandle(retLen);
        char *retBuf = Buffer::Data(ret);
        if (ptr->_firstPass) {
            memcpy(retBuf, ptr->_propBuf, ptr->_propBufSz);
            memcpy(retBuf + ptr->_propBufSz, dst, dstLen);
            ptr->_firstPass = false;
        } else {
            memcpy(retBuf, dst, dstLen);
        }
        
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
    LzmaEnc(int level, int threads) : _firstPass(true), _propBufSz(LZMA_PROPS_SIZE) {
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
    
    bool _firstPass;
    Byte _propBuf[LZMA_PROPS_SIZE];
    size_t _propBufSz;
};

void init(v8::Handle<v8::Object> exports) {
    Local<Object> lzma = NanNew<Object>();
    LzmaTr::setup(lzma);
    exports->Set(NanNew("lzma"), lzma);
}

NODE_MODULE(lzma, init);