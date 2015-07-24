#include <iostream>
#include <nan.h>

#include <Alloc.h>
#include <Ppmd7.h>

#include "stream.h"
#include "buffer_exports.h"
#include "ppmd.h"

using namespace std;

class PpmdHandle {
public:
    PpmdHandle(size_t dicmem, unsigned order) {
        Ppmd7_Construct(&_handle);
        Ppmd7_Alloc(&_handle, dicmem, &g_Alloc);
        Ppmd7_Init(&_handle, order);
    }

    ~PpmdHandle() {
        Ppmd7_Free(&_handle, &g_Alloc);
    }

    operator CPpmd7*() {
        return &_handle;
    }

private:
    CPpmd7 _handle;
};

void readPpmdHead(const char *head, unsigned *order, UInt32 *dicmem) {
    *order = Byte(*head);
    *dicmem = GetUi32(head + 1);
}

void writePpmdHead(char *head, unsigned order, UInt32 dicmem) {
    head[0] = char(Byte(order));
    SetUi32(head + 1, dicmem);
}

void PpmdTr::setup(v8::Handle<v8::Object>& exports) {
    NODE_SET_METHOD(exports, "compress", BufferExports<PpmdTr>::compress);
    NODE_SET_METHOD(exports, "decompress", BufferExports<PpmdTr>::decompress);
}

size_t PpmdTr::getUnpackSize(const char *in, size_t len) {
    if (len <= 5 + 8) return 0;
    size_t unpackSize = GetUi64(in + 5);

    if (unpackSize > 2ULL * 1024 * 1024 * 1024) {
        cout << "Unpack size " << unpackSize << " is too large. Bad input or you should try stream api." << endl;
        return 0;
    }

    return unpackSize;
}

int PpmdTr::decompress(char *out, size_t *outLen, const char *in, size_t *inLen) {
    unsigned order;
    UInt32 dicmem;
    readPpmdHead(in, &order, &dicmem);
    if (order < PPMD7_MIN_ORDER || order > PPMD7_MAX_ORDER || dicmem< PPMD7_MIN_MEM_SIZE || dicmem > PPMD7_MAX_MEM_SIZE) {
        cout << "Bad input data." << endl;
        return SZ_ERROR_DATA;
    }

    PpmdHandle ppmd(dicmem, unsigned(order));
    CPpmd7z_RangeDec desc;
    BufferInStream is(in + 5 + 8, *inLen - 5 - 8);
    Ppmd7z_RangeDec_CreateVTable(&desc);
    desc.Stream = &is;
    Ppmd7z_RangeDec_Init(&desc);

    size_t processed = 0;
    while (!is.overflow() && processed < *outLen) {
        int sym = Ppmd7_DecodeSymbol(ppmd, &desc.p);
        if (sym < 0) break;
        ++processed;
        *out++ = char(sym);
    }

    *outLen = processed;
    return Ppmd7z_RangeDec_IsFinishedOK(&desc) ? 0 : SZ_ERROR_DATA;
}

int PpmdTr::compress(char *out, size_t *outLen, const char *in, size_t inLen, int order, int dicmemMB) {
    if (order < PPMD7_MIN_ORDER || order > PPMD7_MAX_ORDER) order = 8;
    size_t dicmem = size_t(dicmemMB) * 1024 * 1024;
    if (dicmem < PPMD7_MIN_MEM_SIZE || dicmem > PPMD7_MAX_MEM_SIZE) dicmem = 8 * 1024 * 1024;

    size_t propsSize = 5;
    *outLen -= propsSize + 8;

    writePpmdHead(out, unsigned(order), UInt32(dicmem));
    SetUi64(out + propsSize, inLen);

    PpmdHandle ppmd(dicmem, unsigned(order));
    CPpmd7z_RangeEnc desc;
    BufferOutStream os(out +  propsSize + 8, *outLen);
    desc.Stream = &os;
    Ppmd7z_RangeEnc_Init(&desc);
    for (size_t i = 0; i < inLen; i++) {
        Ppmd7_EncodeSymbol(ppmd, &desc, int(Byte(in[i])));
    }
    Ppmd7z_RangeEnc_FlushData(&desc);
    *outLen = os.written() + propsSize + 8;
    return 0;
}
